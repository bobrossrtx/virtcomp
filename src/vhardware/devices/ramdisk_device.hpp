#pragma once

#include "../device.hpp"
#include "../../debug/logger.hpp"

#include <fmt/format.h>
#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <algorithm>

namespace vhw {

/**
 * A virtual RAM disk device for block storage
 * This device uses two ports: one for data and one for control
 * Control commands:
 *   0x00: Set address low byte
 *   0x01: Set address high byte
 *   0x02: Read operation (returns data at current address)
 *   0x03: Write operation (writes data to current address)
 *   0x04: Get size low byte
 *   0x05: Get size high byte
 */
class RamDiskDevice : public VirtualDevice {
public:
    static constexpr uint8_t DEFAULT_DATA_PORT = 0x05;
    static constexpr uint8_t DEFAULT_CTRL_PORT = 0x06;

    // Commands
    static constexpr uint8_t CMD_SET_ADDR_LOW = 0x00;
    static constexpr uint8_t CMD_SET_ADDR_HIGH = 0x01;
    static constexpr uint8_t CMD_READ = 0x02;
    static constexpr uint8_t CMD_WRITE = 0x03;
    static constexpr uint8_t CMD_GET_SIZE_LOW = 0x04;
    static constexpr uint8_t CMD_GET_SIZE_HIGH = 0x05;

    explicit RamDiskDevice(size_t sizeInBytes = 8192)  // Default to 8KB
        : storage(sizeInBytes, 0), currentAddress(0), lastCommand(0), lastData(0) {
    }
    
    ~RamDiskDevice() override = default;
    
    uint8_t read() override {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Handle read based on last command
        if (lastCommand == CMD_READ) {
            if (currentAddress < storage.size()) {
                uint8_t value = storage[currentAddress];
                Logger::instance().debug() << fmt::format(
                    "{:22} │ RamDisk: Read 0x{:02X} from address 0x{:04X}",
                    "", value, currentAddress
                ) << std::endl;
                return value;
            } else {
                Logger::instance().warn() << fmt::format(
                    "RamDisk: Attempted to read from out-of-bounds address 0x{:04X} (size: {})",
                    currentAddress, storage.size()
                ) << std::endl;
            }
            return 0;
        } 
        else if (lastCommand == CMD_GET_SIZE_LOW) {
            return storage.size() & 0xFF;
        }
        else if (lastCommand == CMD_GET_SIZE_HIGH) {
            return (storage.size() >> 8) & 0xFF;
        }
        
        // Default return
        return 0;
    }
    
    void write(uint8_t value) override {
        std::lock_guard<std::mutex> lock(mutex);
        lastData = value;
        
        // If this is the control port, interpret as command
        if (isControlPort) {
            lastCommand = value;
            handleCommand();
        } else {
            // This is the data port, and the last command was CMD_WRITE
            if (lastCommand == CMD_WRITE) {
                if (currentAddress < storage.size()) {
                    storage[currentAddress] = value;
                    Logger::instance().debug() << fmt::format(
                        "{:22} │ RamDisk: Wrote 0x{:02X} to address 0x{:04X}",
                        "", value, currentAddress
                    ) << std::endl;
                } else {
                    Logger::instance().warn() << fmt::format(
                        "RamDisk: Attempted to write to out-of-bounds address 0x{:04X} (size: {})",
                        currentAddress, storage.size()
                    ) << std::endl;
                }
            }
        }
    }
    
    std::string getName() const override {
        return fmt::format("RAM Disk ({} bytes)", storage.size());
    }
    
    void reset() override {
        std::lock_guard<std::mutex> lock(mutex);
        std::fill(storage.begin(), storage.end(), 0);
        currentAddress = 0;
        lastCommand = 0;
        lastData = 0;
    }
    
    /**
     * Set whether this instance is used as a control port
     */
    void setAsControlPort(bool isControl) {
        isControlPort = isControl;
    }
    
    /**
     * Get the current storage content
     */
    std::vector<uint8_t> getStorage() const {
        std::lock_guard<std::mutex> lock(mutex);
        return storage;
    }
    
    /**
     * Set the storage content
     */
    void setStorage(const std::vector<uint8_t>& newStorage) {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Copy as much data as fits
        size_t copySize = std::min(storage.size(), newStorage.size());
        std::copy_n(newStorage.begin(), copySize, storage.begin());
        
        // Fill the rest with zeros if newStorage is smaller
        if (newStorage.size() < storage.size()) {
            std::fill(storage.begin() + copySize, storage.end(), 0);
        }
    }
    
private:
    void handleCommand() {
        switch (lastCommand) {
            case CMD_SET_ADDR_LOW:
                currentAddress = (currentAddress & 0xFF00) | lastData;
                // Validate address bounds
                if (currentAddress >= storage.size()) {
                    Logger::instance().warn() << fmt::format(
                        "RamDisk: Address 0x{:04X} is out of bounds (size: {}), clamping to valid range",
                        currentAddress, storage.size()
                    ) << std::endl;
                    currentAddress = std::min(static_cast<uint16_t>(storage.size() - 1), currentAddress);
                }
                Logger::instance().debug() << fmt::format(
                    "{:14} RamDisk │ Set address low byte to 0x{:02X}, address now 0x{:04X}",
                    "", lastData, currentAddress
                ) << std::endl;
                break;
                
            case CMD_SET_ADDR_HIGH:
                currentAddress = (currentAddress & 0x00FF) | (static_cast<uint16_t>(lastData) << 8);
                // Validate address bounds
                if (currentAddress >= storage.size()) {
                    Logger::instance().warn() << fmt::format(
                        "RamDisk: Address 0x{:04X} is out of bounds (size: {}), clamping to valid range",
                        currentAddress, storage.size()
                    ) << std::endl;
                    currentAddress = std::min(static_cast<uint16_t>(storage.size() - 1), currentAddress);
                }
                Logger::instance().debug() << fmt::format(
                    "{:14} RamDisk │ Set address high byte to 0x{:02X}, address now 0x{:04X}",
                    "", lastData, currentAddress
                ) << std::endl;
                break;
                
            // Other commands are handled in read/write
            default:
                break;
        }
    }
    
    std::vector<uint8_t> storage;
    uint16_t currentAddress;
    uint8_t lastCommand;
    uint8_t lastData;
    bool isControlPort = false;
    mutable std::mutex mutex;
};

} // namespace vhw
