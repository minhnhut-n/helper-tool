#include <stdio.h>
#include <string.h>
#include <time.h>
#include "logging-linux-srv.h"

atomic_ring_buffer_t g_log_ring = {0};

int log_enqueue(uint8_t level, const char* msg) {
    uint32_t head = atomic_load_explicit(&g_log_ring.head, memory_order_relaxed);
    uint32_t tail = atomic_load_explicit(&g_log_ring.tail, memory_order_acquire);

    if (head - tail >= RING_BUFFER_SZ) {
        return -1;
        //drop message
    }

    //circular index
    uint32_t write_idx = head & (RING_BUFFER_SZ-1); 
    g_log_ring.entry[write_idx].level = level;
    g_log_ring.entry[write_idx].timestamp = (uint64_t)time(NULL); //curent unix timestamp

    strncpy(g_log_ring.entry[write_idx].msg, msg, MAX_LOG_LEN-1);
    g_log_ring.entry[write_idx].msg[MAX_LOG_LEN-1] = '\0';

    atomic_store_explicit(&g_log_ring.head, head+1, memory_order_release);
    return 0;
}