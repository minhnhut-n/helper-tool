// stress_log_main.c — stress test đa producer cùng include logger
// Kiểm tra CAS slot-reserve của log_enqueue() có còn mất log không.
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "logging-linux-srv.h"

extern atomic_ring_buffer_t g_log_ring;
extern void run_logging_deamon(void);

#define N_PRODUCERS 4
#define MSGS_PER_PRODUCER 500   // tổng 2000 msg, ring chỉ 1024 -> expect 1 phần bị drop (full) nhưng KHÔNG mất do race

static _Atomic(int) g_enqueued = 0;
static _Atomic(int) g_dropped = 0;

static void *daemon_thread(void *arg) {
    (void)arg;
    run_logging_deamon();
    return NULL;
}

static void *producer_thread(void *arg) {
    long id = (long)arg;
    char buf[MAX_LOG_LEN];
    for (int i = 0; i < MSGS_PER_PRODUCER; i++) {
        snprintf(buf, sizeof(buf), "[fake%d] msg=%d", (int)id, i);
        if (log_enqueue((i % 2) ? LOG_DEBUG : LOG_INFO, buf) == 0)
            atomic_fetch_add(&g_enqueued, 1);
        else
            atomic_fetch_add(&g_dropped, 1);
    }
    return NULL;
}

int main(void) {
    pthread_t daemon, prod[N_PRODUCERS];

    printf("producers=%d x %d = %d total, ring=%d\n",
           N_PRODUCERS, MSGS_PER_PRODUCER,
           N_PRODUCERS * MSGS_PER_PRODUCER, RING_BUFFER_SZ);

    if (pthread_create(&daemon, NULL, daemon_thread, NULL) != 0) {
        perror("pthread_create daemon");
        return 1;
    }
    for (long i = 0; i < N_PRODUCERS; i++) {
        if (pthread_create(&prod[i], NULL, producer_thread, (void *)i) != 0) {
            perror("pthread_create producer");
            return 1;
        }
    }
    for (int i = 0; i < N_PRODUCERS; i++) pthread_join(prod[i], NULL);
    printf("producers done. enqueued=%d dropped=%d\n",
           atomic_load(&g_enqueued), atomic_load(&g_dropped));

    // đợi daemon drain hết ring, tối đa ~10s
    for (int i = 0; i < 1000; i++) {
        uint32_t head = atomic_load_explicit(&g_log_ring.head, memory_order_acquire);
        uint32_t tail = atomic_load_explicit(&g_log_ring.tail, memory_order_acquire);
        if (head == tail) break;
        struct timespec d = { .tv_sec = 0, .tv_nsec = 10 * 1000 * 1000 };
        nanosleep(&d, NULL);
    }

    pthread_cancel(daemon);
    pthread_join(daemon, NULL);
    printf("DONE. check log: grep -a -c '^\\[' %s\n", LOG_FILE_PATH);
    return 0;
}
