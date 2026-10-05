// fake_log_main.c — file .c thứ 3 cùng include logger + điều phối test
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include "logging-linux-srv.h"

extern atomic_ring_buffer_t g_log_ring;
extern void run_logging_deamon(void);

void fake_sensor_dump(int count);
void fake_net_dump(int count);

#define MSGS_PER_MODULE 200

static void *daemon_thread(void *arg) {
    (void)arg;
    run_logging_deamon(); // consumer: drain ring -> LOG_FILE_PATH
    return NULL;
}

static void *sensor_thread(void *arg) {
    (void)arg;
    fake_sensor_dump(MSGS_PER_MODULE);
    return NULL;
}

static void *net_thread(void *arg) {
    (void)arg;
    fake_net_dump(MSGS_PER_MODULE);
    return NULL;
}

int main(void) {
    pthread_t daemon, t1, t2;

    printf("LOG_FILE_PATH = %s\n", LOG_FILE_PATH);
    printf("producers: fake_sensor.c + fake_net.c, each %d msgs (total %d)\n",
           MSGS_PER_MODULE, MSGS_PER_MODULE * 2);

    if (pthread_create(&daemon, NULL, daemon_thread, NULL) != 0) {
        perror("pthread_create daemon");
        return 1;
    }
    if (pthread_create(&t1, NULL, sensor_thread, NULL) != 0 ||
        pthread_create(&t2, NULL, net_thread, NULL) != 0) {
        perror("pthread_create producer");
        return 1;
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("producers done. waiting daemon to drain ring...\n");

    // đợi tới khi head == tail (ring rỗng), tối đa ~5s
    for (int i = 0; i < 500; i++) {
        uint32_t head = atomic_load_explicit(&g_log_ring.head, memory_order_acquire);
        uint32_t tail = atomic_load_explicit(&g_log_ring.tail, memory_order_acquire);
        if (head == tail) break;
        struct timespec d = { .tv_sec = 0, .tv_nsec = 10 * 1000 * 1000 };
        nanosleep(&d, NULL);
    }

    {
        uint32_t head = atomic_load_explicit(&g_log_ring.head, memory_order_acquire);
        uint32_t tail = atomic_load_explicit(&g_log_ring.tail, memory_order_acquire);
        printf("ring: head=%u tail=%u (enqueued=%u)\n", head, tail, head);
        if (head != tail) printf("WARN: ring chưa drain hết (daemon chưa kịp ghi)\n");
    }

    pthread_cancel(daemon);
    pthread_join(daemon, NULL);
    printf("DONE. check log: grep -a . %s | head\n", LOG_FILE_PATH);
    return 0;
}
