#include <stdio.h>
#include "logger-srv.h"

#define SOCK_PATH "/tmp/logger.sock"

int main(void) {
    logger_init(SOCK_PATH);
    logger(LOG_INFO, "hello world!");
    logger_close();
    return 0;
}