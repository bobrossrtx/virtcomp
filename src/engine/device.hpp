#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace vhw {

/**
 * Base interface for all I/O devices, both virtual and real
 */
class Device {
public:
    virtual ~Device() = default;

    // Read a value from the device
    virtual uint8_t read() = 0;

    // Write a value to the device
    virtual void write(uint8_t value) = 0;

    // Get the name of this device
    virtual std::string getName() const = 0;

    // Check if this device is a real hardware device
    virtual bool isRealDevice() const = 0;

    // Reset the device to its initial state
    virtual void reset() = 0;
};

/**
 * Interface for virtual devices that only exist in the VM
 */
class VirtualDevice : public Device {
public:
    bool isRealDevice() const override { return false; }
};

/**
 * Interface for real devices that connect to actual hardware
 */
class RealDevice : public Device {
public:
    bool isRealDevice() const override { return true; }

    // Attempt to connect to the real device
    // Returns true if connection was successful
    virtual bool connect() = 0;

    // Disconnect from the real device
    virtual void disconnect() = 0;

    // Check if the device is currently connected
    virtual bool isConnected() const = 0;
};

} // namespace vhw
