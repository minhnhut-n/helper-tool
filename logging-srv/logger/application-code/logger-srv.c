// src/client/logger.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdarg.h>

#include "logger-srv.h"

static int g_sock_fd = -1;
#define MAX_BUFFER 512

int logger_init(const char *socket_path) {
    g_sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (g_sock_fd < 0) return -1;

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (connect(g_sock_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(g_sock_fd);
        g_sock_fd = -1;
        return -1;
    }
    return 0;
}

void logger(log_type_t level, const char* fmt, ...) {
    if (g_sock_fd < 0) return;

    char buffer[MAX_BUFFER];
    int headLen = snprintf(buffer, sizeof(buffer), "%d:", level);

    va_list args;
    va_start(args, fmt);
    int bodyLen = vsnprintf(buffer+headLen, MAX_BUFFER-1-headLen, fmt, args);
    va_end(args);

    int totalLen = headLen+bodyLen;
    if (totalLen >= MAX_BUFFER-1) totalLen = MAX_BUFFER-2;
    
    buffer[totalLen++] = '\n';
    buffer[totalLen] = '\0';
    
    // send log to socket -> event with be creae and background task on 
    // daemon will collect it to ring buffer
    write(g_sock_fd, buffer, totalLen);
}

void logger_close(void) {
    if (g_sock_fd >= 0) {
        close(g_sock_fd);
        g_sock_fd = -1;
    }
}