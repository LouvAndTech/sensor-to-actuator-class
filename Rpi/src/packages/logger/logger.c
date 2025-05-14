#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#define DIR_PATH "./logs"
#define BASE_FILENAME "log_"

const char* LOG_LEVEL_STRINGS[] = {
    "DEBUG", 
    "INFO", 
    "WARNING",
    "ERROR"
};

// Private function prototypes
static void create_log_directory();
static FILE* create_log_file(const char* filename);

#include <stdarg.h>

// Public functions
void logger_log(LOG_LEVEL level, const char* format, ...) {
    if (level < DEBUG || level > ERROR) {
        fprintf(stderr, "Invalid log level: %d\n", level);
        return;
    }

    // Create the log directory if it doesn't exist
    create_log_directory();

    // Create the log file with the appropriate name
    char filename[256];
    snprintf(filename, sizeof(filename), "%s%s.txt", BASE_FILENAME, LOG_LEVEL_STRINGS[level]);
    FILE* file = create_log_file(filename);
    if (file == NULL) {
        return;
    }

    // Add timestamp to the log message
    time_t now = time(NULL);
    struct tm* local_time = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time);

    // Format the user message
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    // Write the log message to the file
    fprintf(file, "[%s] [%s] %s\n", timestamp, LOG_LEVEL_STRINGS[level], message);
    fflush(file); // Ensure the message is written immediately
    fclose(file); // Close the file
}

// Private function to create the log directory if it doesn't exist
static void create_log_directory() {
    if (access(DIR_PATH, F_OK) == -1) {
        if (mkdir(DIR_PATH, 0755) == -1) {
            perror("Error creating log directory");
        }
    }
}

// Private function to create the log file if it doesn't exist
static FILE* create_log_file(const char* filename) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s", DIR_PATH, filename);
    FILE* file = fopen(filepath, "a");
    if (file == NULL) {
        perror("Error opening log file");
        return NULL;
    }
    return file;
}