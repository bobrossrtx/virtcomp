#pragma once

#include "../device.hpp"
#include "../../debug/logger.hpp"

#include <fmt/format.h>
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <mutex>
#include <filesystem>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdexcept>

namespace fs = std::filesystem;
namespace vhw {

/**
 * A virtual device that reads/writes to a file
 * Reading will get a byte from the file
 * Writing will append a byte to the file
 */
class FileDevice : public VirtualDevice {
public:
    static constexpr uint8_t DEFAULT_PORT = 0x04;

    explicit FileDevice(const std::string& filepath)
        : filepath(filepath), position(0) {
        // Validate file path for security before proceeding
        if (!validateFilePath(filepath)) {
            throw std::runtime_error("Invalid or unsafe file path: " + filepath);
        }
        
        // Try to open the file for reading
        loadFromFile();
    }

    ~FileDevice() override = default;    uint8_t read() override {
        std::lock_guard<std::mutex> lock(mutex);

        // If position is at the end of the buffer or beyond, return 0
        if (position >= fileBuffer.size()) {
            return 0;
        }

        // Validate position before incrementing to prevent overflow
        if (position == SIZE_MAX) {
            Logger::instance().warn() << fmt::format(
                "File device position overflow at maximum value, resetting to 0"
            ) << std::endl;
            position = 0;
            return 0;
        }

        // Return byte at current position and advance
        return fileBuffer[position++];
    }

    void write(uint8_t value) override {
        std::lock_guard<std::mutex> lock(mutex);

        // If writing at the current position or beyond the end, append
        if (position >= fileBuffer.size()) {
            fileBuffer.push_back(value);
            position = fileBuffer.size();
        } else {
            // Otherwise update existing byte
            fileBuffer[position++] = value;
        }

        // Save to file
        saveToFile();
    }

    std::string getName() const override {
        return fmt::format("File Device ({})", filepath);
    }

    void reset() override {
        std::lock_guard<std::mutex> lock(mutex);
        position = 0;
        loadFromFile();
    }

    /**
     * Set the file position
     */
    void seek(size_t newPosition) {
        std::lock_guard<std::mutex> lock(mutex);
        position = std::min(newPosition, fileBuffer.size());
    }

    /**
     * Get the current file position
     */
    size_t tell() const {
        return position;
    }

    /**
     * Get the file size
     */
    size_t size() const {
        return fileBuffer.size();
    }

private:
    /**
     * Validate file path for security (prevents symlink attacks, path traversal, etc.)
     */
    bool validateFilePath(const std::string& path) {
        // Check if path is empty or too long
        if (path.empty() || path.length() >= PATH_MAX) {
            Logger::instance().error() << fmt::format(
                "File path is empty or too long: '{}'", path
            ) << std::endl;
            return false;
        }

        // Check for path traversal attempts
        if (path.find("..") != std::string::npos) {
            Logger::instance().error() << fmt::format(
                "File path contains path traversal: '{}'", path
            ) << std::endl;
            return false;
        }

        // Check for absolute paths to sensitive directories
        const std::vector<std::string> forbiddenPaths = {
            "/etc/",
            "/proc/",
            "/sys/",
            "/dev/",
            "/boot/",
            "/root/",
            "/var/log/"
        };

        for (const auto& forbidden : forbiddenPaths) {
            if (path.find(forbidden) == 0) {
                Logger::instance().error() << fmt::format(
                    "File path accesses forbidden directory: '{}'", path
                ) << std::endl;
                return false;
            }
        }

        // Get the directory path and check if it exists and is safe
        fs::path fullPath = fs::absolute(path);
        fs::path parentPath = fullPath.parent_path();

        // Check if parent directory exists and is not a symlink
        if (fs::exists(parentPath)) {
            struct stat st;
            if (lstat(parentPath.c_str(), &st) == 0) {
                if (S_ISLNK(st.st_mode)) {
                    Logger::instance().error() << fmt::format(
                        "File path parent directory is a symbolic link (security risk): '{}'",
                        parentPath.string()
                    ) << std::endl;
                    return false;
                }
            }
        }

        // If file exists, check if it's a symlink or special file
        if (fs::exists(fullPath)) {
            struct stat st;
            if (lstat(fullPath.c_str(), &st) == 0) {
                if (S_ISLNK(st.st_mode)) {
                    Logger::instance().error() << fmt::format(
                        "File path is a symbolic link (security risk): '{}'", path
                    ) << std::endl;
                    return false;
                }

                // Only allow regular files
                if (!S_ISREG(st.st_mode)) {
                    Logger::instance().error() << fmt::format(
                        "File path is not a regular file: '{}'", path
                    ) << std::endl;
                    return false;
                }
            }
        }

        return true;
    }

    void loadFromFile() {
        fileBuffer.clear();

        // Validate file path for security
        if (!validateFilePath(filepath)) {
            Logger::instance().error() << fmt::format(
                "Invalid or unsafe file path: '{}'", filepath
            ) << std::endl;
            return;
        }

        // Try to open the file and read its contents
        std::ifstream file(filepath, std::ios::binary);
        if (file) {
            // Read file into buffer with size validation
            file.seekg(0, std::ios::end);
            auto fileSizePos = file.tellg();
            file.seekg(0, std::ios::beg);

            // Validate file size to prevent excessive memory allocation
            if (fileSizePos < 0) {
                Logger::instance().error() << fmt::format(
                    "Error getting file size for '{}'", filepath
                ) << std::endl;
                return;
            }

            size_t fileSize = static_cast<size_t>(fileSizePos);

            // Limit maximum file size to prevent memory exhaustion (e.g., 100MB)
            constexpr size_t MAX_FILE_SIZE = 100 * 1024 * 1024;
            if (fileSize > MAX_FILE_SIZE) {
                Logger::instance().error() << fmt::format(
                    "File '{}' is too large ({} bytes, max: {} bytes)",
                    filepath, fileSize, MAX_FILE_SIZE
                ) << std::endl;
                return;
            }

            fileBuffer.resize(fileSize);
            if (fileSize > 0) {
                file.read(reinterpret_cast<char*>(fileBuffer.data()), fileSize);

                // Check if the read was successful
                if (!file || file.gcount() != static_cast<std::streamsize>(fileSize)) {
                    Logger::instance().error() << fmt::format(
                        "Error reading file '{}': expected {} bytes, read {} bytes",
                        filepath, fileSize, file.gcount()
                    ) << std::endl;
                    fileBuffer.clear();
                    return;
                }
            }

            Logger::instance().info() << fmt::format(
                "Loaded {} bytes from file '{}'",
                fileSize, filepath
            ) << std::endl;
        } else {
            Logger::instance().info() << fmt::format(
                "Creating new file '{}'",
                filepath
            ) << std::endl;
        }
    }

    void saveToFile() {
        // Validate file path for security before writing
        if (!validateFilePath(filepath)) {
            Logger::instance().error() << fmt::format(
                "Invalid or unsafe file path for writing: '{}'", filepath
            ) << std::endl;
            return;
        }

        // Create directories if needed (but validate the path first)
        fs::path path(filepath);
        if (path.has_parent_path()) {
            try {
                fs::create_directories(path.parent_path());
            } catch (const fs::filesystem_error& e) {
                Logger::instance().error() << fmt::format(
                    "Failed to create directories for '{}': {}", filepath, e.what()
                ) << std::endl;
                return;
            }
        }

        // Write buffer to file
        std::ofstream file(filepath, std::ios::binary);
        if (file) {
            file.write(reinterpret_cast<const char*>(fileBuffer.data()), fileBuffer.size());
            Logger::instance().debug() << fmt::format(
            "{:22} │ Wrote {} bytes to file '{}'",
            "", fileBuffer.size(), filepath
            ) << std::endl;
        } else {
            Logger::instance().error() << fmt::format(
            "{:22} │ Failed to write to file '{}'",
            "", filepath
            ) << std::endl;
        }
    }

    std::string filepath;
    std::vector<uint8_t> fileBuffer;
    size_t position;
    std::mutex mutex;
};

} // namespace vhw
