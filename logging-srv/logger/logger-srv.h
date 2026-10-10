/** For public method logging
 * Owner: minhnhut.n
 * Date: 10/10/2026
 */
#ifndef _LOGGER_SERVICE_
#define _LOGGER_SERVICE_

typedef enum {
    LOG_INFO,
    LOG_DEBUG,
    LOG_ERROR,
    LOG_FORCE
} log_type_t;

/**
 * for initial logger service with 2 options
 * option 1: default with arg= void, using available tty0
 * option 2: specific a tty serial port to export
 */
int logger_init(void);
int logger_init(const char* tty_dev);

/**
 * method to write log, with ring buffer machanism
 * level: info/debug/force(tty)/error(force)
 * msg  : const message can not modify
 */
void logger_ring(log_type_t level, const char* msg);

/**
 * method to write log, with socket machanism
 * level: info/debug/force(tty)/error(force)
 * msg  : const message can not modify
 */
void logger_socket(log_type_t level, const char* msg);

#endif