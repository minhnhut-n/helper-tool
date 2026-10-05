#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <time.h>
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

//open file and write
static int write_log_entry(int file_fd, off_t *file_offset,
                           const log_entry_t *log_entry) {
    // init
    char line[MAX_LOG_LEN + 64];
    int line_length = snprintf(
        line, sizeof(line), "[%llu] [%s] %s\n",
        (unsigned long long)log_entry->timestamp,
        log_entry->level == LOG_INFO ? "INFO" : "DEBUG",
        log_entry->msg);

    // check & setting
    if (line_length < 0 || (size_t)line_length >= sizeof(line)) {
        return -1;
    }

    if ((size_t)line_length > LOG_FILE_SIZE) {
        return -1;
    }

    if (*file_offset + (off_t)line_length > (off_t)LOG_FILE_SIZE) {
        *file_offset = 0;
    }

    const char *write_ptr = line;
    if (lseek(file_fd, *file_offset, SEEK_SET) < 0) {
        return -1;
    }

    //perform action
    while (line_length > 0) {
        //function write with pointer set
        ssize_t written = write(file_fd, write_ptr, (size_t)line_length);
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
        write_ptr += written;
        line_length -= (int)written;
    }

    return 0;
}

//logging at background
void run_logging_deamon(void) {
    // local copy: KHÔNG copy nguyên struct (chứa _Atomic), chỉ copy payload
    uint64_t ts;
    uint8_t lv;
    char msg[MAX_LOG_LEN];
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
            const struct timespec delay = {
                .tv_sec = 0,
                .tv_nsec = 10 * 1000 * 1000
            };
            struct timespec remaining = delay;

            while (nanosleep(&remaining, &remaining) == -1 && errno == EINTR) {
                // Continue waiting
            }
            continue;
        }

        uint32_t print_idx = tail & (RING_BUFFER_SZ-1);

        // đợi producer ghi xong slot này (publish qua committed, acquire để thấy msg)
        if (atomic_load_explicit(&g_log_ring.entry[print_idx].committed,
                                 memory_order_acquire) == 0) {
            const struct timespec spin = { .tv_sec = 0, .tv_nsec = 1000 }; // 1us
            nanosleep(&spin, NULL);
            continue; // chưa tăng tail -> thử lại slot này
        }

        ts = g_log_ring.entry[print_idx].timestamp;
        lv = g_log_ring.entry[print_idx].level;
        memcpy(msg, g_log_ring.entry[print_idx].msg, MAX_LOG_LEN);
        msg[MAX_LOG_LEN - 1] = '\0';

        // reset cờ + giải phóng slot cho vòng sau
        atomic_store_explicit(&g_log_ring.entry[print_idx].committed, 0, memory_order_relaxed);
        atomic_store_explicit(&g_log_ring.tail, tail+1, memory_order_release);

        log_entry_t log_entry;
        log_entry.timestamp = ts;
        log_entry.level = lv;
        memcpy(log_entry.msg, msg, MAX_LOG_LEN);

        if (write_log_entry(file_fd, &file_offset, &log_entry) != 0) {
            perror("cannot write log file");
        }
    }
}