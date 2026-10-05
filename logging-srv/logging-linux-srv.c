#include <stdio.h>
#include <string.h>
#include <time.h>
#include "logging-linux-srv.h"

atomic_ring_buffer_t g_log_ring = {0};

int log_enqueue(uint8_t level, const char* msg) {
    uint32_t head, tail;

    // compare and swap, CAS
    // implementation with slot reserve for preventing override log
    do {
        head = atomic_load_explicit(&g_log_ring.head, memory_order_relaxed);
        tail = atomic_load_explicit(&g_log_ring.tail, memory_order_acquire);

        if (head - tail >= RING_BUFFER_SZ) {
            return -1;
            //drop message
        }
    } while (!atomic_compare_exchange_weak_explicit(&g_log_ring.head, &head, head + 1, memory_order_acquire, memory_order_relaxed));
    //slot reserve successfully (head đã tăng đúng 1 lần qua CAS, KHÔNG store thêm).

    //circular index
    uint32_t write_idx = head & (RING_BUFFER_SZ-1);
    g_log_ring.entry[write_idx].level = level;
    g_log_ring.entry[write_idx].timestamp = (uint64_t)time(NULL); //curent unix timestamp

    strncpy(g_log_ring.entry[write_idx].msg, msg, MAX_LOG_LEN-1);
    g_log_ring.entry[write_idx].msg[MAX_LOG_LEN-1] = '\0';

    // publish: báo consumer slot này đã ghi xong (giữ thứ tự với release)
    atomic_store_explicit(&g_log_ring.entry[write_idx].committed, 1, memory_order_release);
    return 0;
}