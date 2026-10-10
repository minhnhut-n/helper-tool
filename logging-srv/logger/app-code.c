
#define PIPE_PATH "/var"
static int g_log_fd = -1;

int logger_init(void) {
    g_log_fd =  open(PIPE_PATH, O_WRONLY | O_NONBLOCK);
    return g_log_fd;
}

void function(log_type_t level, const char* msg) {
    if (g_log_fd < 0) return;
    char buffer[BUFFER_SIZE];
    int len = sprintf(buffer, "[%d]: %s", level, msg);

    ssize_t written = write(g_log_fd, buffer, len);
    if (written < 0) { 
        // fail handle
    }
}