#pragma once

#include "../device.hpp"
#include "../../debug/logger.hpp"

#include <fmt/format.h>
#include <iostream>
#include <cstdint>
#include <string>
#include <deque>

using Logging::Logger;

namespace vhw {

/**
 * A simple virtual console device for text I/O
 * Reading will get a character from the input buffer (or 0 if empty)
 * Writing will output the character to the console
 */
class ConsoleDevice : public VirtualDevice {
public:
    static constexpr uint8_t DEFAULT_PORT = 0x01;

    ConsoleDevice() = default;
    ~ConsoleDevice() override = default;

    uint8_t read() override {
        std::lock_guard<std::mutex> lock(bufferMutex);
        if (inputBuffer.empty()) {
            return 0;
        }

        uint8_t value = inputBuffer.front();
        inputBuffer.pop_front();
        return value;
    }

    void write(uint8_t value) override {
        // Output the character to stdout
        std::cout << static_cast<char>(value) << std::flush;

        // Also log it
        Logger::instance().debug() << fmt::format(
            "Console output: {} ('{}')",
            value,
            value >= 32 && value < 127 ? fmt::format("{}", static_cast<char>(value)) : "."
        ) << std::endl;
    }

    std::string getName() const override {
        return "Virtual Console";
    }

    void reset() override {
        std::lock_guard<std::mutex> lock(bufferMutex);
        inputBuffer.clear();
    }

    /**
     * Add a character to the input buffer
     * This would be called by the system when a key is pressed
     */
    void addInput(uint8_t value) {
        std::lock_guard<std::mutex> lock(bufferMutex);
        inputBuffer.push_back(value);
    }

    /**
     * Add a string to the input buffer
     * This is a convenience method for testing
     */
    void addInput(const std::string& input) {
        std::lock_guard<std::mutex> lock(bufferMutex);
        for (char c : input) {
            inputBuffer.push_back(static_cast<uint8_t>(c));
        }
    }

private:
    std::deque<uint8_t> inputBuffer;
    std::mutex bufferMutex;
};

} // namespace vhw
