#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>
#include <string.h>

#include "logger-srv.h"
#define BUFFER_SIZE 1024;

typedef struct {
    _Atomic((uint32_t)) head;
    _Atomic((uint32_t)) tail;
    string ring_slot[BUFFER_SIZE];
} ring_package_t;

ring_package_t g_ring;

// luồng ghi
void logger(log_type_t level, const char* msg) {

    // cơ chế ring buffer, ghi tại head, đọc tại tail, tránh gây race condition
    uint32_t head, tail;
    // đọc head, tail
    head = atomic_load_explicit(&g_ring.head, memory_order_relaxed); // luồng ghi 
    tail = atomic_load_explicit(&g_ring.tail, memory_order_acquire); // luồng đọc

    if ((head-tail) > BUFFER_SIZE) {
        // buffer is full, drop message.
        return;
        // not permitted to overwrite on tail from head
        // another thread is reading
    }

    int write_idx = head & (BUFFER_SIZE-1);
    // lấy đủ thông tin của ring (tạm thời chỉ có string)   
    sprintf(g_ring.ring_slot[write_idx], "%s", msg);
    
    // ghi ring và cập nhật vào general memory space
    atomic_store_explicit(&g_ring.head, head+1, memory_order_release);

    return 0;
}