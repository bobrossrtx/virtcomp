#pragma once

#include "../device.hpp"
#include "../../debug/logger.hpp"

#include <fmt/format.h>
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <deque>
#include <mutex>
#include <filesystem>

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
        // Try to open the file for reading
        loadFromFile();
    }
    
    ~FileDevice() override = default;
    
    uint8_t read() override {
        std::lock_guard<std::mutex> lock(mutex);
        
        // If position is at the end of the buffer or beyond, return 0
        if (position >= fileBuffer.size()) {
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
    void loadFromFile() {
        fileBuffer.clear();
        
        // Try to open the file and read its contents
        std::ifstream file(filepath, std::ios::binary);
        if (file) {
            // Read file into buffer
            file.seekg(0, std::ios::end);
            size_t fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            
            fileBuffer.resize(fileSize);
            if (fileSize > 0) {
                file.read(reinterpret_cast<char*>(fileBuffer.data()), fileSize);
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
        // Create directories if needed
        fs::path path(filepath);
        if (path.has_parent_path()) {
            fs::create_directories(path.parent_path());
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
