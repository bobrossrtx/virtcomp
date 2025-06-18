#pragma once

#include "../device.hpp"
#include "../../debug/logger.hpp"

#include <fmt/format.h>
#include <cstdint>
#include <string>
#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <atomic>

// Unix-specific headers
#ifndef _WIN32
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>
#include <libgen.h>
#endif

namespace vhw {

/**
 * A real device that communicates with a serial port
 * This is a basic implementation - real application would need more error handling
 */
class SerialPortDevice : public RealDevice {
public:
    static constexpr uint8_t DEFAULT_PORT = 0x03;

    explicit SerialPortDevice(const std::string& portName)
        : portName(portName), fd(-1), connected(false), readThread(nullptr), running(false) {}

    ~SerialPortDevice() override {
        if (isConnected()) {
            disconnect();
        }
    }

    bool connect() override {
        #ifdef _WIN32
        // Windows implementation would use CreateFile, SetupComm, etc.
        Logger::instance().error() << "Windows serial port support not implemented yet" << std::endl;
        return false;
        #else
        // Validate the port path for security
        if (!validateSerialPortPath(portName)) {
            Logger::instance().error() << fmt::format(
                "Invalid or unsafe serial port path: '{}'", portName
            ) << std::endl;
            return false;
        }

        // Unix implementation using termios
        // Open port with secure flags (O_NOFOLLOW prevents symlink attacks)
        fd = ::open(portName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK | O_NOFOLLOW);
        if (fd < 0) {
            Logger::instance().error() << fmt::format(
                "Failed to open serial port '{}': {}",
                portName, strerror(errno)
            ) << std::endl;
            return false;
        }

        // CRITICAL: Re-validate after opening to prevent TOCTOU race conditions
        // An attacker could have changed the file between validation and opening
        if (!validateOpenedSerialPort(fd, portName)) {
            Logger::instance().error() << fmt::format(
                "Serial port validation failed after opening (possible TOCTOU attack): '{}'",
                portName
            ) << std::endl;
            ::close(fd);
            fd = -1;
            return false;
        }

        // Configure port (simple configuration for demo)
        struct termios tty;
        memset(&tty, 0, sizeof(tty));
        if (tcgetattr(fd, &tty) != 0) {
            Logger::instance().error() << fmt::format(
                "Failed to get serial port attributes: {}",
                strerror(errno)
            ) << std::endl;
            ::close(fd);
            fd = -1;
            return false;
        }

        // Set baud rate to 9600
        cfsetispeed(&tty, B9600);
        cfsetospeed(&tty, B9600);

        // 8N1 mode, no flow control
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        tty.c_cflag &= ~CRTSCTS;
        tty.c_cflag |= CREAD | CLOCAL;

        // Raw input mode
        tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

        // Raw output mode
        tty.c_oflag &= ~OPOST;

        // Set attributes
        if (tcsetattr(fd, TCSANOW, &tty) != 0) {
            Logger::instance().error() << fmt::format(
                "Failed to set serial port attributes: {}",
                strerror(errno)
            ) << std::endl;
            ::close(fd);
            fd = -1;
            return false;
        }

        // Start read thread
        running = true;
        readThread = std::make_unique<std::thread>(&SerialPortDevice::readLoop, this);

        connected = true;
        Logger::instance().info() << fmt::format(
            "Connected to serial port '{}'",
            portName
        ) << std::endl;
        return true;
        #endif
    }

    void disconnect() override {
        #ifdef _WIN32
        // Windows implementation would use CloseHandle
        #else
        // Stop read thread
        running = false;
        if (readThread && readThread->joinable()) {
            readThread->join();
            readThread.reset();
        }

        // Close port
        if (fd >= 0) {
            ::close(fd);
            fd = -1;
        }
        #endif

        connected = false;
        Logger::instance().info() << fmt::format(
            "Disconnected from serial port '{}'",
            portName
        ) << std::endl;
    }

    bool isConnected() const override {
        return connected;
    }

    uint8_t read() override {
        std::lock_guard<std::mutex> lock(bufferMutex);
        if (rxBuffer.empty()) {
            return 0;
        }

        uint8_t value = rxBuffer.front();
        rxBuffer.pop_front();
        return value;
    }

    void write(uint8_t value) override {
        if (!isConnected()) {
            Logger::instance().warn() << fmt::format(
                "Attempted to write to disconnected serial port: {}",
                value
            ) << std::endl;
            return;
        }

        #ifdef _WIN32
        // Windows implementation would use WriteFile
        #else
        // Unix implementation using write
        ssize_t written = ::write(fd, &value, 1);
        if (written != 1) {
            Logger::instance().error() << fmt::format(
                "Failed to write to serial port: {} (written: {})",
                strerror(errno), written
            ) << std::endl;
        }
        #endif
    }

    std::string getName() const override {
        return fmt::format("Serial Port ({})", portName);
    }

    void reset() override {
        std::lock_guard<std::mutex> lock(bufferMutex);
        rxBuffer.clear();
    }

private:
    /**
     * Validate opened file descriptor for security (prevents TOCTOU race conditions)
     * This function validates the already-opened file descriptor to ensure it matches
     * our security requirements, preventing Time-of-Check-Time-of-Use attacks.
     */
    bool validateOpenedSerialPort(int fd, const std::string& path) {
        #ifndef _WIN32
        struct stat st;

        // Use fstat on the opened file descriptor (not the path)
        // This eliminates TOCTOU race conditions
        if (fstat(fd, &st) != 0) {
            Logger::instance().error() << fmt::format(
                "Failed to stat opened serial port '{}': {}",
                path, strerror(errno)
            ) << std::endl;
            return false;
        }

        // Verify it's still a character device
        if (!S_ISCHR(st.st_mode)) {
            Logger::instance().error() << fmt::format(
                "Opened file '{}' is not a character device (possible TOCTOU attack)",
                path
            ) << std::endl;
            return false;
        }

        // Additional security checks on the opened file descriptor

        // Check if the device has reasonable permissions
        // Most serial devices should be readable/writable by owner or group
        mode_t perms = st.st_mode & 0777;
        if ((perms & 0600) == 0) {  // At least owner read/write
            Logger::instance().warn() << fmt::format(
                "Serial port '{}' has unusual permissions: {:o}", path, perms
            ) << std::endl;
        }

        // Check if the device is a block device (which would be suspicious)
        if (S_ISBLK(st.st_mode)) {
            Logger::instance().error() << fmt::format(
                "Opened file '{}' is a block device (not allowed for serial ports)", path
            ) << std::endl;
            return false;
        }

        // Check if it's a regular file (which would be suspicious for a serial port)
        if (S_ISREG(st.st_mode)) {
            Logger::instance().error() << fmt::format(
                "Opened file '{}' is a regular file (not allowed for serial ports)", path
            ) << std::endl;
            return false;
        }

        // For additional security, we could check the device major/minor numbers
        // to ensure they match expected serial device ranges, but this is system-specific

        return true;
        #else
        // Windows validation would go here
        (void)fd;    // Suppress unused parameter warning
        (void)path;  // Suppress unused parameter warning
        return true;
        #endif
    }

    /**
     * Validate serial port path for security (prevents symlink attacks, device file attacks, etc.)
     */
    bool validateSerialPortPath(const std::string& path) {
        #ifndef _WIN32
        // Check if path is empty or too long
        if (path.empty() || path.length() >= PATH_MAX) {
            Logger::instance().error() << fmt::format(
                "Serial port path is empty or too long: '{}'", path
            ) << std::endl;
            return false;
        }

        // Only allow specific serial device patterns for security
        const std::vector<std::string> allowedPrefixes = {
            "/dev/ttyS",     // Standard serial ports
            "/dev/ttyUSB",   // USB serial adapters
            "/dev/ttyACM",   // CDC ACM devices
            "/dev/ttyAMA",   // ARM serial ports
            "/dev/ttymxc",   // i.MX serial ports
            "/dev/serial/by-id/",  // Persistent names by ID
            "/dev/serial/by-path/" // Persistent names by path
        };

        bool isAllowed = false;
        for (const auto& prefix : allowedPrefixes) {
            if (path.find(prefix) == 0) {
                isAllowed = true;
                break;
            }
        }

        if (!isAllowed) {
            Logger::instance().error() << fmt::format(
                "Serial port path not in allowed list: '{}'", path
            ) << std::endl;
            return false;
        }

        // Check for path traversal attempts
        if (path.find("..") != std::string::npos) {
            Logger::instance().error() << fmt::format(
                "Serial port path contains path traversal: '{}'", path
            ) << std::endl;
            return false;
        }

        // Use lstat to detect symlinks (more secure than stat)
        struct stat st;
        if (lstat(path.c_str(), &st) != 0) {
            Logger::instance().error() << fmt::format(
                "Cannot stat serial port path '{}': {}", path, strerror(errno)
            ) << std::endl;
            return false;
        }

        // Reject symbolic links for security
        if (S_ISLNK(st.st_mode)) {
            Logger::instance().error() << fmt::format(
                "Serial port path is a symbolic link (security risk): '{}'", path
            ) << std::endl;
            return false;
        }

        // Must be a character device
        if (!S_ISCHR(st.st_mode)) {
            Logger::instance().error() << fmt::format(
                "Serial port path is not a character device: '{}'", path
            ) << std::endl;
            return false;
        }

        return true;
        #else
        // Windows validation would go here
        return true;
        #endif
    }

    void readLoop() {
        #ifndef _WIN32
        uint8_t buffer[256];
        while (running) {
            ssize_t bytesRead = ::read(fd, buffer, sizeof(buffer));
            if (bytesRead > 0) {
                std::lock_guard<std::mutex> lock(bufferMutex);

                // Prevent buffer overflow by limiting rxBuffer size
                constexpr size_t MAX_RX_BUFFER_SIZE = 4096;  // 4KB limit

                for (ssize_t i = 0; i < bytesRead; ++i) {
                    if (rxBuffer.size() >= MAX_RX_BUFFER_SIZE) {
                        // Buffer full, drop oldest data to make room
                        Logger::instance().warn() << fmt::format(
                            "Serial port RX buffer full ({} bytes), dropping oldest data",
                            MAX_RX_BUFFER_SIZE
                        ) << std::endl;
                        rxBuffer.pop_front();
                    }
                    rxBuffer.push_back(buffer[i]);
                }
            } else if (bytesRead < 0 && errno != EAGAIN) {
                Logger::instance().error() << fmt::format(
                    "Error reading from serial port: {}",
                    strerror(errno)
                ) << std::endl;
            }

            // Sleep a bit to prevent 100% CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        #endif
    }

    std::string portName;
    int fd;  // File descriptor for the serial port
    std::atomic<bool> connected;

    std::deque<uint8_t> rxBuffer;
    std::mutex bufferMutex;

    std::unique_ptr<std::thread> readThread;
    std::atomic<bool> running;
};

} // namespace vhw
