// src/client/logger.c
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

static int g_sock_fd = -1;

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

void logger(int level, const char *message) {
    if (g_sock_fd < 0) return;

    char buffer[512];
    int len = snprintf(buffer, sizeof(buffer), "%d: %s\n", level, message);
    
    // send log to socket -> event with be creae and background task on 
    // daemon will collect it to ring buffer
    write(g_sock_fd, buffer, len);
}

void logger_close(void) {
    if (g_sock_fd >= 0) {
        close(g_sock_fd);
        g_sock_fd = -1;
    }
}