#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
} LOG_LEVEL;

/**
 * Log a message with the specified log level.
 * @param level The log level (DEBUG, INFO, WARNING, ERROR).
 * @param message The message to log.
 */
void logger_log(LOG_LEVEL level, const char* format, ...);

#endif // LOGGER_H