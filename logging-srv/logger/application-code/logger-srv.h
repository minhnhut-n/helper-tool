/** Public interface for application code
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
 */
int logger_init(const char *sock_path);

/**
 * method to write log, with ring buffer machanism
 * level: info/debug/force(tty)/error(force)
 * msg  : const message can not modify
 */
void logger(log_type_t level, const char* msg);

/**
 * method to close connect
 */
void logger_close(void);

#endif