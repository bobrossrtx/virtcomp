#pragma once

#include "device.hpp"
#include "../debug/logger.hpp"

#include <fmt/format.h>
#include <cstdint>
#include <unordered_map>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>

namespace vhw {

/**
 * Manages all I/O devices and handles mapping between ports and devices
 */
class DeviceManager {
public:
    static DeviceManager& instance() {
        static DeviceManager instance;
        return instance;
    }

    /**
     * Register a device at a specific port
     * @param port The port number to register the device at
     * @param device The device to register
     * @throws std::runtime_error if a device is already registered at the port
     */
    void registerDevice(uint8_t port, std::shared_ptr<Device> device) {
        if (devices.find(port) != devices.end()) {
            throw std::runtime_error(
                fmt::format("Error: Port {} already has a device registered ({})",
                port, devices[port]->getName())
            );
        }

        devices[port] = device;
        Logger::instance().info() << fmt::format(
            "Device '{}' registered at port {}",
            device->getName(), port
        ) << std::endl;
    }

    /**
     * Unregister a device from a port
     * @param port The port to unregister the device from
     * @return true if a device was unregistered, false if no device was at the port
     */
    bool unregisterDevice(uint8_t port) {
        auto it = devices.find(port);
        if (it == devices.end()) {
            return false;
        }

        Logger::instance().info() << fmt::format(
            "Device '{}' unregistered from port {}",
            it->second->getName(), port
        ) << std::endl;

        // If it's a real device, make sure to disconnect it
        auto realDevice = std::dynamic_pointer_cast<RealDevice>(it->second);
        if (realDevice && realDevice->isConnected()) {
            realDevice->disconnect();
        }

        devices.erase(it);
        return true;
    }

    /**
     * Get a device at a specific port
     * @param port The port to get the device from
     * @return The device at the port, or nullptr if no device is registered
     */
    std::shared_ptr<Device> getDevice(uint8_t port) const {
        auto it = devices.find(port);
        if (it == devices.end()) {
            return nullptr;
        }
        return it->second;
    }

    /**
     * Read a value from a device at a specific port
     * @param port The port to read from
     * @return The value read from the device, or 0 if no device is registered
     */
    uint8_t readPort(uint8_t port) {
        auto device = getDevice(port);
        if (!device) {
            Logger::instance().warn() << fmt::format(
                "Attempted to read from unregistered port {}, returning 0",
                port
            ) << std::endl;
            return 0;
        }

        // For real devices, check if they're connected
        auto realDevice = std::dynamic_pointer_cast<RealDevice>(device);
        if (realDevice && !realDevice->isConnected()) {
            Logger::instance().warn() << fmt::format(
                "Real device '{}' at port {} is not connected, returning 0",
                realDevice->getName(), port
            ) << std::endl;
            return 0;
        }

        uint8_t value = device->read();
        Logger::instance().debug() << fmt::format(
            "{:>23}│ Input from port {} ({}): {}",
            "", port, device->getName(), value
        ) << std::endl;

        return value;
    }

    /**
     * Write a value to a device at a specific port
     * @param port The port to write to
     * @param value The value to write
     */
    void writePort(uint8_t port, uint8_t value) {
        auto device = getDevice(port);
        if (!device) {
            Logger::instance().warn() << fmt::format(
                "Attempted to write to unregistered port {}, value: {}",
                port, value
            ) << std::endl;
            return;
        }

        // For real devices, check if they're connected
        auto realDevice = std::dynamic_pointer_cast<RealDevice>(device);
        if (realDevice && !realDevice->isConnected()) {
            Logger::instance().warn() << fmt::format(
                "Real device '{}' at port {} is not connected, ignoring write of value {}",
                realDevice->getName(), port, value
            ) << std::endl;
            return;
        }

        Logger::instance().debug() << fmt::format(
            "{:>23}│ Output to port {} ({}): {}",
            "", port, device->getName(), value
        ) << std::endl;

        device->write(value);
    }

    /**
     * Reset all registered devices
     */
    void resetAllDevices() {
        Logger::instance().info() << "Resetting all devices..." << std::endl;
        for (auto& [port, device] : devices) {
            device->reset();
        }
    }

    /**
     * Get all registered device ports
     * @return A vector of port numbers that have registered devices
     */
    std::vector<uint8_t> getRegisteredPorts() const {
        std::vector<uint8_t> ports;
        for (const auto& [port, _] : devices) {
            ports.push_back(port);
        }
        return ports;
    }

    void reset() {
        Logger::instance().info() << "Resetting DeviceManager..." << std::endl;
        resetAllDevices();
        devices.clear();
        Logger::instance().info() << "DeviceManager reset complete." << std::endl;
    }

    // Port read/write methods for different data sizes
    // Note: Multi-byte operations check for port overflow to prevent buffer issues
    uint8_t readPortByte(uint8_t port);
    void writePortByte(uint8_t port, uint8_t value);
    uint16_t readPortWord(uint8_t port);  // Requires ports [port, port+1]
    void writePortWord(uint8_t port, uint16_t value);  // Requires ports [port, port+1]
    uint32_t readPortDWord(uint8_t port);  // Requires ports [port, port+3]
    void writePortDWord(uint8_t port, uint32_t value);  // Requires ports [port, port+3]

    // String I/O methods with bounds checking
    std::string readPortString(uint8_t port, uint8_t maxLength = 255);
    void writePortString(uint8_t port, const std::string& str);

private:
    DeviceManager() = default;
    ~DeviceManager() {
        // Ensure all real devices are disconnected
        for (auto& [port, device] : devices) {
            auto realDevice = std::dynamic_pointer_cast<RealDevice>(device);
            if (realDevice && realDevice->isConnected()) {
                realDevice->disconnect();
            }
        }
    }

    // Delete copy constructor and assignment operator
    DeviceManager(const DeviceManager&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;

    std::unordered_map<uint8_t, std::shared_ptr<Device>> devices;
};

} // namespace vhw
