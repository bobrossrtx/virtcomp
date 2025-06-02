#include "device_manager.hpp"

namespace vhw {

uint8_t DeviceManager::readPortByte(uint8_t port) {
    return readPort(port);
}

void DeviceManager::writePortByte(uint8_t port, uint8_t value) {
    writePort(port, value);
}

uint16_t DeviceManager::readPortWord(uint8_t port) {
    uint8_t lo = readPort(port);
    uint8_t hi = readPort(port + 1);
    return static_cast<uint16_t>(lo) | (static_cast<uint16_t>(hi) << 8);
}

void DeviceManager::writePortWord(uint8_t port, uint16_t value) {
    writePort(port, static_cast<uint8_t>(value & 0xFF));
    writePort(port + 1, static_cast<uint8_t>((value >> 8) & 0xFF));
}

uint32_t DeviceManager::readPortDWord(uint8_t port) {
    uint32_t result = 0;
    for (int i = 0; i < 4; ++i)
        result |= (static_cast<uint32_t>(readPort(port + i)) << (8 * i));
    return result;
}

void DeviceManager::writePortDWord(uint8_t port, uint32_t value) {
    for (int i = 0; i < 4; ++i)
        writePort(port + i, static_cast<uint8_t>((value >> (8 * i)) & 0xFF));
}

std::string DeviceManager::readPortString(uint8_t port, uint8_t maxLength) {
    std::string result;
    result.reserve(maxLength);
    
    for (uint8_t i = 0; i < maxLength; ++i) {
        uint8_t byte = readPort(port);
        if (byte == 0) {
            // Null terminator found
            break;
        }
        result.push_back(static_cast<char>(byte));
    }
    
    Logger::instance().debug() << fmt::format(
        "{:>23}│ String input from port {}: \"{}\" ({} bytes)",
        "", port, result, result.length()
    ) << std::endl;
    
    return result;
}

void DeviceManager::writePortString(uint8_t port, const std::string& str) {
    Logger::instance().debug() << fmt::format(
        "{:>23}│ String output to port {}: \"{}\" ({} bytes)",
        "", port, str, str.length()
    ) << std::endl;
    
    for (char c : str) {
        writePort(port, static_cast<uint8_t>(c));
    }
    
    // Write null terminator
    writePort(port, 0);
}

} // namespace vhw