#include <errno.h>
#include <string.h>
#include "./packages/logger/logger.h"

// Clean \n in string for logger
#define CLEAN_STRING(str) \
    do { \
        for (int i = 0; i < strlen(str); i++) \
            str[i] = (str[i] == '\n') ? ' ' : str[i]; \
    } while(0)

//Log macros
#define LOG_ERROR(fmt, ...) \
    do { \
        fprintf(stderr,"ERROR: " fmt "\n", ##__VA_ARGS__); \
        logger_log(ERROR,fmt, ##__VA_ARGS__); \
    } while(0)

#define LOG_PERROR(fmt, ...) \
    do { \
        perror("ERROR:" fmt); \
        logger_log(ERROR, "%s: %s", fmt, strerror(errno)); \
    } while(0)

#define LOG_WARN(fmt, ...) \
    do { \
        fprintf(stderr,"WARNING: " fmt "\n", ##__VA_ARGS__); \
        logger_log(WARNING, fmt, ##__VA_ARGS__); \
    } while(0)

#define LOG_INFO(fmt, ...) \
    do { \
        fprintf(stdout,"INFO: " fmt "\n", ##__VA_ARGS__); \
        logger_log(INFO, fmt, ##__VA_ARGS__); \
    } while(0)

#define LOG_DEBUG(fmt, ...) \
    do { \
        fprintf(stdout,"DEBUG: " fmt "\n", ##__VA_ARGS__); \
        logger_log(DEBUG, fmt, ##__VA_ARGS__); \
    } while(0)
