#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include <iomanip>
#include <filesystem>
#include <vector>
#include <chrono>

#include <fmt/core.h>

#include "../config.hpp"

enum class LogLevel { SUCCESS, ERRORINFO, INFO, WARNING, ERROR, DEBUG, RUNNING, VIRTCOMP };

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

    // Force next message to bypass filtering (verbose/debug checks)
    Logger& force() {
        force_next_ = true;
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
    }    void log(LogLevel level, const std::string& msg) {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        std::ostream& out = (level == LogLevel::ERROR) ? std::cerr : std::cout;
        std::string color = level_to_color(level);
        std::string reset = "\033[0m";

        // Debug mode guard (bypass if forced)
        if (level == LogLevel::DEBUG && !Config::debug && !force_next_) {
            force_next_ = false; // Reset force flag
            return;
        }

        // Verbose mode guard for info messages (bypass if forced)
        if (level == LogLevel::INFO && !Config::verbose && !force_next_) {
            force_next_ = false; // Reset force flag
            return;
        }

        // VirtComp messages always print (no filtering)
        // Note: VIRTCOMP level bypasses verbose/debug checks

        // Reset force flag after checking
        force_next_ = false;

        // Get current date and time with milliseconds
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        std::time_t t = std::chrono::system_clock::to_time_t(now);        std::tm tm;
    #if defined(_WIN32)
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif
        // Use a safer buffer size and check for potential overflow
        constexpr size_t datetime_buffer_size = 64;
        char datetime[datetime_buffer_size];
        size_t result = std::strftime(datetime, datetime_buffer_size, "%y-%m-%d %H:%M:%S", &tm);
        if (result == 0) {
            // strftime failed, use a fallback
            std::strcpy(datetime, "00-00-00 00:00:00");
        }
        std::ostringstream datetime_with_ms;
        datetime_with_ms << datetime << "." << std::setfill('0') << std::setw(3) << ms.count();
        std::string datetime_str = datetime_with_ms.str();

        std::string log_line;
        if (level == LogLevel::WARNING) {
            log_line = fmt::format("[{}] [{}] {:>20} │ {}", datetime_str, level_to_string(level), "", msg);
        } else {
            log_line = "[" + datetime_str + "] [" + level_to_string(level) + "] " + msg;
        }

        out << color << log_line << reset << std::endl;

        // Write to file if enabled
        if (file_.is_open()) {
            file_ << log_line << std::endl;
            file_.flush();
        }

        // Add to GUI buffer
        {
            std::lock_guard<std::recursive_mutex> lock(gui_log_mutex_);
            gui_log_buffer_.push_back(log_line);
            if (gui_log_buffer_.size() > gui_log_buffer_max_)
                gui_log_buffer_.erase(gui_log_buffer_.begin());
        }
    }

    // Convenience methods
    Logger& success()   { return level(LogLevel::SUCCESS);  }
    Logger& info()      { return level(LogLevel::INFO);     }
    Logger& warn()      { return level(LogLevel::WARNING);  }
    Logger& debug()     { return level(LogLevel::DEBUG);    }
    Logger& running()   { return level(LogLevel::RUNNING);  }
    Logger& virtcomp()  { return level(LogLevel::VIRTCOMP); }
    Logger& error(const std::string& extra_info = ""){
        Config::error_count++;

        if (!extra_info.empty()) {
            buffer_ << " (" << extra_info << ")";
        }
        return level(LogLevel::ERROR);
    }

    std::vector<std::string> get_gui_log_buffer() {
        std::lock_guard<std::recursive_mutex> lock(gui_log_mutex_);
        return gui_log_buffer_;
    }

    void clear_gui_log_buffer() {
        std::lock_guard<std::recursive_mutex> lock(gui_log_mutex_);
        gui_log_buffer_.clear();
    }

private:
    Logger() {
        // Open debug_file from config.hpp, clear contents if exists
        if (!Config::debug_file.empty()) {
            file_.open(Config::debug_file, std::ios::out | std::ios::trunc);
        }
    }
    std::ostringstream buffer_;
    LogLevel current_level_ = LogLevel::INFO;
    bool force_next_ = false; // Flag to bypass filtering for next message
    std::ofstream file_;
    std::recursive_mutex mutex_;

    std::vector<std::string> gui_log_buffer_;
    static constexpr size_t gui_log_buffer_max_ = 500;
    std::recursive_mutex gui_log_mutex_;    std::string level_to_string(LogLevel level) {
        switch (level) {
            case LogLevel::SUCCESS:     return "SUCCESS";
            case LogLevel::INFO:        return "INFO";
            case LogLevel::WARNING:     return "WARNING";
            case LogLevel::ERROR:       return "ERROR";
            case LogLevel::DEBUG:       return "DEBUG";
            case LogLevel::RUNNING:     return "RUNNING";
            case LogLevel::VIRTCOMP:    return "VIRTCOMP";
            default:                    return "LOG";
        }
    }    std::string level_to_color(LogLevel level) {
        switch (level) {
            case LogLevel::SUCCESS:     return "\033[1;32m";     // Bright Green
            case LogLevel::INFO:        return "\033[1;36m";     // Bright Cyan
            case LogLevel::WARNING:     return "\033[1;33m";     // Bright Yellow
            case LogLevel::ERROR:       return "\033[1;31m";     // Bright Red
            case LogLevel::ERRORINFO:   return "\033[1;36m";     // Bright Cyan
            case LogLevel::DEBUG:       return "\033[38;5;208m"; // Orange (ANSI 256-color)
            case LogLevel::RUNNING:     return "\033[1;34m";     // Bright Blue
            case LogLevel::VIRTCOMP:    return "\033[1;35m";     // Bright Magenta (readable purple)
            default:                    return "\033[0m";
        }
    }
};