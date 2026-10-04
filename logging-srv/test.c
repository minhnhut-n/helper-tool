#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include "logging-linux-srv.h"

extern atomic_ring_buffer_t g_log_ring;
extern void run_logging_deamon(void);

static void *daemon_thread(void *arg) {
    (void)arg;
    run_logging_deamon();
    return NULL;
}

int main(void) {
    pthread_t thread;

    if (pthread_create(&thread, NULL, daemon_thread, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }

    if (log_enqueue(LOG_INFO, "TEST logger INFO") != 0 ||
        log_enqueue(LOG_DEBUG, "TEST logger DEBUG") != 0) {
        fprintf(stderr, "log_enqueue failed\n");
        pthread_cancel(thread);
        pthread_join(thread, NULL);
        return 1;
    }

    /* Đợi daemon lấy hết log khỏi ring buffer. */
    for (int i = 0; i < 200; ++i) {
        uint32_t head = atomic_load_explicit(&g_log_ring.head, memory_order_acquire);
        uint32_t tail = atomic_load_explicit(&g_log_ring.tail, memory_order_acquire);
        if (head == tail) {
            break;
        }

        const struct timespec delay = { .tv_sec = 0, .tv_nsec = 10 * 1000 * 1000 };
        nanosleep(&delay, NULL);
    }

    pthread_cancel(thread);
    pthread_join(thread, NULL);
    puts("Test logs enqueued; check /tmp/logging.log");
    return 0;
}