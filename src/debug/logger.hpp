#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include <iomanip>
#include <filesystem>
#include "../config.hpp"

enum class LogLevel { SUCCESS, ERRORINFO, INFO, WARNING, ERROR, DEBUG };

class Logger {
public:
    static Logger& instance() {
        static Logger inst;
        return inst;
    }

    // Set log level for next message
    Logger& level(LogLevel lvl) {
        current_level_ = lvl;
        buffer_.str(""); // Clear buffer
        return *this;
    }

    // Overload << for generic types
    template<typename T>
    Logger& operator<<(const T& val) {
        buffer_ << val;
        return *this;
    }

    // Overload << for std::endl and flush
    Logger& operator<<(std::ostream& (*manip)(std::ostream&)) {
        if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl)) {
            log(current_level_, buffer_.str());
            buffer_.str("");
        }
        return *this;
    }

    void log(LogLevel level, const std::string& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::ostream& out = (level == LogLevel::ERROR) ? std::cerr : std::cout;
        std::string color = level_to_color(level);
        std::string reset = "\033[0m";

        // Get current date and time with milliseconds
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
    #if defined(_WIN32)
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif
        char datetime[40];
        std::strftime(datetime, sizeof(datetime), "%y-%m-%d | %H:%M:%S", &tm);
        std::ostringstream datetime_with_ms;
        datetime_with_ms << datetime << "." << std::setfill('0') << std::setw(3) << ms.count();
        std::string datetime_str = datetime_with_ms.str();

        std::string log_line = "[" + datetime_str + "] [" + level_to_string(level) + "] " + msg;

        out << color << log_line << reset << std::endl;

        // Write to file if enabled
        if (file_.is_open()) {
            file_ << log_line << std::endl;
            file_.flush();
        }
    }

    // Convenience methods
    Logger& success() { return level(LogLevel::SUCCESS); }
    Logger& info()    { return level(LogLevel::INFO); }
    Logger& warn()    { return level(LogLevel::WARNING); }
    Logger& error(const std::string& extra_info = "") {
        error_count++;

        if (!extra_info.empty()) {
            buffer_ << " (" << extra_info << ")";
        }
        return level(LogLevel::ERROR);
    }
    Logger& debug()   { return level(LogLevel::DEBUG); }

private:
    Logger() {
        // Open debug_file from config.hpp, clear contents if exists
        if (!debug_file.empty()) {
            file_.open(debug_file, std::ios::out | std::ios::trunc);
        }
    }
    std::ostringstream buffer_;
    LogLevel current_level_ = LogLevel::INFO;
    std::ofstream file_;
    std::mutex mutex_;

    std::string level_to_string(LogLevel level) {
        switch (level) {
            case LogLevel::SUCCESS: return "SUCCESS";
            case LogLevel::INFO:    return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR:   return "ERROR";
            case LogLevel::DEBUG:   return "DEBUG";
            default:                return "LOG";
        }
    }

    std::string level_to_color(LogLevel level) {
        switch (level) {
            case LogLevel::SUCCESS:     return "\033[1;32m";     // Bright Green
            case LogLevel::INFO:        return "\033[1;36m";     // Bright Cyan
            case LogLevel::WARNING:     return "\033[1;33m";     // Bright Yellow
            case LogLevel::ERROR:       return "\033[1;31m";     // Bright Red
            case LogLevel::ERRORINFO:   return "\033[1;36m";     // Bright Cyan
            case LogLevel::DEBUG:       return "\033[38;5;208m"; // Orange (ANSI 256-color)
            default:                    return "\033[0m";
        }
    }
};