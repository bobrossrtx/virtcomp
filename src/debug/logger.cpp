#include "logger.hpp"

#include <chrono>
#include <cstring>
#include <ctime>
#include <iomanip>

#include <fmt/core.h>

#include "../config.hpp"

namespace Logging {

// ============================================================================
// Singleton Access
// ============================================================================

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

// ============================================================================
// Constructor and Destructor
// ============================================================================

Logger::Logger()
    : current_level_(LogLevel::INFO)
    , force_next_(false)
    , file_logging_enabled_(true) {

    // Initialize log file if specified in config
    if (!Config::debug_file.empty()) {
        set_log_file(Config::debug_file);
    }
}

Logger::~Logger() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

// ============================================================================
// Core Logging Interface
// ============================================================================

Logger& Logger::level(LogLevel lvl) {
    current_level_ = lvl;
    buffer_.str(""); // Clear buffer
    buffer_.clear(); // Clear any error flags
    return *this;
}

Logger& Logger::force() {
    force_next_ = true;
    return *this;
}

Logger& Logger::operator<<(std::ostream& (*manip)(std::ostream&)) {
    if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl)) {
        log(current_level_, buffer_.str());
        buffer_.str("");
        buffer_.clear();
    }
    return *this;
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::recursive_mutex> lock(console_mutex_);

    // Check if message should be filtered
    if (should_filter_message(level)) {
        force_next_ = false; // Reset force flag
        return;
    }

    // Reset force flag after checking
    force_next_ = false;

    // Generate timestamp
    std::string timestamp = generate_timestamp();

    // Format the complete log line
    std::string formatted_message = format_log_line(level, message, timestamp);

    // Output to console
    write_to_console(level, formatted_message);

    // Write to file if enabled
    if (file_logging_enabled_) {
        write_to_file(formatted_message);
    }

    // Add to GUI buffer
    add_to_gui_buffer(formatted_message);
}

// ============================================================================
// Convenience Methods
// ============================================================================

Logger& Logger::success() {
    return level(LogLevel::SUCCESS);
}

Logger& Logger::info() {
    return level(LogLevel::INFO);
}

Logger& Logger::warn() {
    return level(LogLevel::WARNING);
}

Logger& Logger::error(const std::string& extra_info) {
    Config::error_count++;

    if (!extra_info.empty()) {
        buffer_ << " (" << extra_info << ")";
    }
    return level(LogLevel::ERROR);
}

Logger& Logger::debug() {
    return level(LogLevel::DEBUG);
}

Logger& Logger::running() {
    return level(LogLevel::RUNNING);
}

Logger& Logger::virtcomp() {
    return level(LogLevel::VIRTCOMP);
}

// ============================================================================
// GUI Buffer Management
// ============================================================================

std::vector<std::string> Logger::get_gui_log_buffer() const {
    std::lock_guard<std::recursive_mutex> lock(gui_mutex_);
    return gui_log_buffer_;
}

void Logger::clear_gui_log_buffer() {
    std::lock_guard<std::recursive_mutex> lock(gui_mutex_);
    gui_log_buffer_.clear();
}

size_t Logger::get_gui_buffer_size() const {
    std::lock_guard<std::recursive_mutex> lock(gui_mutex_);
    return gui_log_buffer_.size();
}

// ============================================================================
// File Logging Control
// ============================================================================

void Logger::set_file_logging_enabled(bool enabled) {
    std::lock_guard<std::recursive_mutex> lock(console_mutex_);
    file_logging_enabled_ = enabled;
}

bool Logger::is_file_logging_enabled() const {
    std::lock_guard<std::recursive_mutex> lock(console_mutex_);
    return file_logging_enabled_;
}

bool Logger::set_log_file(const std::string& file_path) {
    std::lock_guard<std::recursive_mutex> lock(console_mutex_);

    // Close existing file if open
    if (log_file_.is_open()) {
        log_file_.close();
    }

    // Try to open new file
    log_file_.open(file_path, std::ios::out | std::ios::trunc);

    if (log_file_.is_open()) {
        // Write header to log file
        log_file_ << "=== VirtComp Log Session Started ===" << std::endl;
        log_file_ << "Timestamp: " << generate_timestamp() << std::endl;
        log_file_ << "========================================" << std::endl;
        log_file_.flush();
        return true;
    }

    return false;
}

// ============================================================================
// Helper Methods
// ============================================================================

std::string Logger::level_to_string(LogLevel level) const {
    switch (level) {
        case LogLevel::SUCCESS:     return "SUCCESS";
        case LogLevel::INFO:        return "INFO";
        case LogLevel::WARNING:     return "WARNING";
        case LogLevel::ERROR:       return "ERROR";
        case LogLevel::DEBUG:       return "DEBUG";
        case LogLevel::RUNNING:     return "RUNNING";
        case LogLevel::VIRTCOMP:    return "VIRTCOMP";
        case LogLevel::ERRORINFO:   return "ERRORINFO";
        default:                    return "UNKNOWN";
    }
}

std::string Logger::level_to_color(LogLevel level) const {
    switch (level) {
        case LogLevel::SUCCESS:     return "\033[1;32m";     // Bright Green
        case LogLevel::INFO:        return "\033[1;36m";     // Bright Cyan
        case LogLevel::WARNING:     return "\033[1;33m";     // Bright Yellow
        case LogLevel::ERROR:       return "\033[1;31m";     // Bright Red
        case LogLevel::ERRORINFO:   return "\033[1;36m";     // Bright Cyan
        case LogLevel::DEBUG:       return "\033[38;5;208m"; // Orange (ANSI 256-color)
        case LogLevel::RUNNING:     return "\033[1;34m";     // Bright Blue
        case LogLevel::VIRTCOMP:    return "\033[1;35m";     // Bright Magenta
        default:                    return RESET_COLOR;
    }
}

std::string Logger::generate_timestamp() const {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::tm tm;
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    // Create timestamp with milliseconds
    char datetime[DATETIME_BUFFER_SIZE];
    size_t result = std::strftime(datetime, DATETIME_BUFFER_SIZE,
                                  "%y-%m-%d %H:%M:%S", &tm);

    if (result == 0) {
        // strftime failed, use fallback
        std::strcpy(datetime, "00-00-00 00:00:00");
    }

    std::ostringstream datetime_with_ms;
    datetime_with_ms << datetime << "."
                     << std::setfill('0') << std::setw(3) << ms.count();

    return datetime_with_ms.str();
}

std::string Logger::format_log_line(LogLevel level, const std::string& message,
                                   const std::string& timestamp) const {
    std::string level_str = level_to_string(level);

    // Special formatting for WARNING messages
    if (level == LogLevel::WARNING) {
        return fmt::format("[{}] [{}] {:>20} │ {}",
                          timestamp, level_str, "", message);
    } else {
        return fmt::format("[{}] [{}] {}",
                          timestamp, level_str, message);
    }
}

bool Logger::should_filter_message(LogLevel level) const {
    // If force flag is set, never filter
    if (force_next_) {
        return false;
    }

    // Never filter ERROR, SUCCESS, WARNING, RUNNING, VIRTCOMP, or ERRORINFO
    switch (level) {
        case LogLevel::ERROR:
        case LogLevel::SUCCESS:
        case LogLevel::WARNING:
        case LogLevel::RUNNING:
        case LogLevel::VIRTCOMP:
        case LogLevel::ERRORINFO:
            return false;

        case LogLevel::DEBUG:
            return !Config::debug;

        case LogLevel::INFO:
            return !Config::verbose;

        default:
            return false;
    }
}

void Logger::write_to_console(LogLevel level, const std::string& formatted_message) const {
    // Determine output stream
    std::ostream& out = (level == LogLevel::ERROR) ? std::cerr : std::cout;

    // Apply color and output
    std::string color = level_to_color(level);
    out << color << formatted_message << RESET_COLOR << std::endl;
}

void Logger::write_to_file(const std::string& formatted_message) {
    if (log_file_.is_open()) {
        log_file_ << formatted_message << std::endl;
        log_file_.flush();
    }
}

void Logger::add_to_gui_buffer(const std::string& formatted_message) {
    std::lock_guard<std::recursive_mutex> lock(gui_mutex_);

    gui_log_buffer_.push_back(formatted_message);

    // Maintain buffer size limit
    if (gui_log_buffer_.size() > GUI_LOG_BUFFER_MAX) {
        gui_log_buffer_.erase(gui_log_buffer_.begin());
    }
}

} // namespace Logging
