#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

namespace Logging {

/**
 * @brief Enumeration of available log levels
 *
 * Log levels are ordered by severity, with SUCCESS being the lowest
 * and ERROR being the highest. Special levels like VIRTCOMP bypass
 * normal filtering rules.
 */
enum class LogLevel {
    SUCCESS,    ///< Success messages (green)
    INFO,       ///< Informational messages (cyan) - filtered by verbose mode
    WARNING,    ///< Warning messages (yellow)
    ERROR,      ///< Error messages (red) - always shown
    DEBUG,      ///< Debug messages (orange) - filtered by debug mode
    RUNNING,    ///< Process status messages (blue)
    VIRTCOMP,   ///< VirtComp system messages (magenta) - always shown
    ERRORINFO   ///< Error-related info (cyan) - always shown
};

/**
 * @brief Thread-safe singleton logger with multiple output targets
 *
 * The Logger class provides a centralized logging system that supports:
 * - Multiple log levels with color-coded console output
 * - File logging with automatic timestamping
 * - GUI buffer for in-application log display
 * - Thread-safe operations with mutex protection
 * - Configurable filtering based on debug and verbose modes
 *
 * Usage examples:
 * @code
 * Logger::instance().info() << "Information message" << std::endl;
 * Logger::instance().error("context") << "Error occurred" << std::endl;
 * Logger::instance().force().debug() << "Forced debug message" << std::endl;
 * @endcode
 */
class Logger {
public:
    // Constants
    static constexpr size_t GUI_LOG_BUFFER_MAX = 500;
    static constexpr size_t DATETIME_BUFFER_SIZE = 64;

    /**
     * @brief Get the singleton instance of the Logger
     * @return Reference to the Logger instance
     */
    static Logger& instance();

    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // ============================================================================
    // Core Logging Interface
    // ============================================================================

    /**
     * @brief Set the log level for the next message
     * @param lvl The log level to set
     * @return Reference to this Logger for method chaining
     */
    Logger& level(LogLevel lvl);

    /**
     * @brief Force the next message to bypass filtering rules
     * @return Reference to this Logger for method chaining
     */
    Logger& force();

    /**
     * @brief Stream operator for logging arbitrary types
     * @tparam T Type to be logged
     * @param val Value to be logged
     * @return Reference to this Logger for method chaining
     */
    template<typename T>
    Logger& operator<<(const T& val) {
        buffer_ << val;
        return *this;
    }

    /**
     * @brief Stream operator for handling std::endl and other manipulators
     * @param manip Stream manipulator function
     * @return Reference to this Logger for method chaining
     */
    Logger& operator<<(std::ostream& (*manip)(std::ostream&));

    /**
     * @brief Core logging function that handles message output
     * @param level The log level of the message
     * @param message The message content to log
     */
    void log(LogLevel level, const std::string& message);

    // ============================================================================
    // Convenience Methods
    // ============================================================================

    /**
     * @brief Set log level to SUCCESS
     * @return Reference to this Logger for method chaining
     */
    Logger& success();

    /**
     * @brief Set log level to INFO
     * @return Reference to this Logger for method chaining
     */
    Logger& info();

    /**
     * @brief Set log level to WARNING
     * @return Reference to this Logger for method chaining
     */
    Logger& warn();

    /**
     * @brief Set log level to ERROR and increment error count
     * @param extra_info Optional additional context information
     * @return Reference to this Logger for method chaining
     */
    Logger& error(const std::string& extra_info = "");

    /**
     * @brief Set log level to DEBUG
     * @return Reference to this Logger for method chaining
     */
    Logger& debug();

    /**
     * @brief Set log level to RUNNING
     * @return Reference to this Logger for method chaining
     */
    Logger& running();

    /**
     * @brief Set log level to VIRTCOMP
     * @return Reference to this Logger for method chaining
     */
    Logger& virtcomp();

    // ============================================================================
    // GUI Buffer Management
    // ============================================================================

    /**
     * @brief Get a copy of the current GUI log buffer
     * @return Vector containing all buffered log messages
     */
    std::vector<std::string> get_gui_log_buffer() const;

    /**
     * @brief Clear all messages from the GUI log buffer
     */
    void clear_gui_log_buffer();

    /**
     * @brief Get the current size of the GUI log buffer
     * @return Number of messages in the buffer
     */
    size_t get_gui_buffer_size() const;

    // ============================================================================
    // File Logging Control
    // ============================================================================

    /**
     * @brief Enable or disable file logging
     * @param enabled Whether file logging should be enabled
     */
    void set_file_logging_enabled(bool enabled);

    /**
     * @brief Check if file logging is currently enabled
     * @return True if file logging is enabled, false otherwise
     */
    bool is_file_logging_enabled() const;

    /**
     * @brief Set a custom log file path
     * @param file_path Path to the log file
     * @return True if file was successfully opened, false otherwise
     */
    bool set_log_file(const std::string& file_path);

private:
    /**
     * @brief Private constructor for singleton pattern
     */
    Logger();

    /**
     * @brief Destructor ensures proper cleanup
     */
    ~Logger();

    // ============================================================================
    // Helper Methods
    // ============================================================================

    /**
     * @brief Convert log level to string representation
     * @param level The log level to convert
     * @return String representation of the log level
     */
    std::string level_to_string(LogLevel level) const;

    /**
     * @brief Convert log level to ANSI color code
     * @param level The log level to convert
     * @return ANSI color code string
     */
    std::string level_to_color(LogLevel level) const;

    /**
     * @brief Generate formatted timestamp string with milliseconds
     * @return Formatted timestamp string
     */
    std::string generate_timestamp() const;

    /**
     * @brief Format log message with appropriate styling
     * @param level The log level
     * @param message The message content
     * @param timestamp The timestamp string
     * @return Formatted log line
     */
    std::string format_log_line(LogLevel level, const std::string& message,
                                const std::string& timestamp) const;

    /**
     * @brief Check if a message should be filtered based on current settings
     * @param level The log level to check
     * @return True if message should be filtered (not shown), false otherwise
     */
    bool should_filter_message(LogLevel level) const;

    /**
     * @brief Write message to console with appropriate coloring
     * @param level The log level
     * @param formatted_message The formatted message to output
     */
    void write_to_console(LogLevel level, const std::string& formatted_message) const;

    /**
     * @brief Write message to log file if enabled
     * @param formatted_message The formatted message to write
     */
    void write_to_file(const std::string& formatted_message);

    /**
     * @brief Add message to GUI buffer with size management
     * @param formatted_message The formatted message to add
     */
    void add_to_gui_buffer(const std::string& formatted_message);

    // ============================================================================
    // Member Variables
    // ============================================================================

    std::ostringstream buffer_;              ///< Buffer for building log messages
    LogLevel current_level_;                 ///< Current log level for next message
    bool force_next_;                        ///< Flag to bypass filtering for next message

    std::ofstream log_file_;                 ///< Output file stream for file logging
    bool file_logging_enabled_;              ///< Whether file logging is enabled
    mutable std::recursive_mutex console_mutex_; ///< Mutex for console output synchronization

    std::vector<std::string> gui_log_buffer_; ///< Buffer for GUI log display
    mutable std::recursive_mutex gui_mutex_;  ///< Mutex for GUI buffer synchronization

    // ANSI color codes
    static constexpr const char* RESET_COLOR = "\033[0m";
};

} // namespace Logging