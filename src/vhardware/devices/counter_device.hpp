#pragma once

#include "../device.hpp"

#include <fmt/format.h>
#include <cstdint>
#include <string>

namespace vhw {

/**
 * A simple virtual counter device for testing
 * Reading returns the current counter value
 * Writing increments the counter by the given value
 */
class CounterDevice : public VirtualDevice {
public:
    static constexpr uint8_t DEFAULT_PORT = 0x02;

    CounterDevice() : counter(0) {}
    ~CounterDevice() override = default;
    
    uint8_t read() override {
        return counter;
    }
    
    void write(uint8_t value) override {
        counter += value;
    }
    
    std::string getName() const override {
        return "Virtual Counter";
    }
    
    void reset() override {
        counter = 0;
    }
    
    /**
     * Get the current counter value
     */
    uint8_t getCounter() const {
        return counter;
    }
    
    /**
     * Set the counter to a specific value
     */
    void setCounter(uint8_t value) {
        counter = value;
    }
    
private:
    uint8_t counter;
};

} // namespace vhw
