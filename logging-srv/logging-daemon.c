#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "logging-linux-srv.h"

extern atomic_ring_buffer_t g_log_ring;

static int open_log_file(void) {
    int file_fd = open(LOG_FILE_PATH, O_CREAT | O_RDWR, 0640);
    if (file_fd < 0) {
        return -1;
    }

    if (ftruncate(file_fd, LOG_FILE_SIZE) != 0) {
        close(file_fd);
        return -1;
    }

    return file_fd;
}

static int write_log_entry(int file_fd, off_t *file_offset,
                           const log_entry_t *log_entry) {
    char line[MAX_LOG_LEN + 64];
    int line_length = snprintf(
        line, sizeof(line), "[%llu] [%s] %s\n",
        (unsigned long long)log_entry->timestamp,
        log_entry->level == LOG_INFO ? "INFO" : "DEBUG",
        log_entry->msg);

    if (line_length < 0 || (size_t)line_length >= sizeof(line)) {
        return -1;
    }

    if ((size_t)line_length > LOG_FILE_SIZE) {
        return -1;
    }

    if (*file_offset + line_length > LOG_FILE_SIZE) {
        *file_offset = 0;
    }

    while (line_length > 0) {
        ssize_t written = pwrite(file_fd, line, (size_t)line_length,
                                 *file_offset);
        if (written < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (written == 0) {
            return -1;
        }

        *file_offset += written;
        line += written;
        line_length -= (int)written;
    }

    return 0;
}

//logging at background
void run_logging_deamon(void) {
    log_entry_t log_entry;
    off_t file_offset = 0;
    int file_fd = open_log_file();

    if (file_fd < 0) {
        perror("cannot open log file");
        return;
    }

    while (1) {
        uint32_t head = atomic_load_explicit(&g_log_ring.head, memory_order_acquire);
        uint32_t tail = atomic_load_explicit(&g_log_ring.tail, memory_order_relaxed);

        if (tail == head) {
            usleep(10000); //10ms
            continue;
        }

        uint32_t print_idx = tail & (RING_BUFFER_SZ-1);
        log_entry = g_log_ring.entry[print_idx];

        atomic_store_explicit(&g_log_ring.tail, tail+1, memory_order_release);

        if (write_log_entry(file_fd, &file_offset, &log_entry) != 0) {
            perror("cannot write log file");
        }
    }
}