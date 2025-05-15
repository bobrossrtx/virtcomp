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
        // Unix implementation using termios
        // Open port
        fd = ::open(portName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd < 0) {
            Logger::instance().error() << fmt::format(
                "Failed to open serial port '{}': {}", 
                portName, strerror(errno)
            ) << std::endl;
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
    void readLoop() {
        #ifndef _WIN32
        uint8_t buffer[256];
        while (running) {
            ssize_t bytesRead = ::read(fd, buffer, sizeof(buffer));
            if (bytesRead > 0) {
                std::lock_guard<std::mutex> lock(bufferMutex);
                for (ssize_t i = 0; i < bytesRead; ++i) {
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
