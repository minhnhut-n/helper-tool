#include "ring_buffer.h"

#define BUFFER_SIZE 1024;

static int ring_buffer_push(log_ring_buffer_t *rb, int level, const char *msg) {
    uint32_t curr_head = atomic_load_explicit(&rb->head, memory_order_relaxed);
    uint32_t curr_tail = atomic_load_explicit(&rb->tail, memory_order_acquire);

    if ((curr_head - curr_tail) > BUFFER_SIZE) {
        //drop message, buffer now is full (read is not catch up with write)
        return -1;
    }

    //assign message struct to buffer
    rb->buffer[curr_head & (BUFFER_SIZE - 1)].level = level;
    snprintf(rb->buffer[curr_head & (BUFFER_SIZE - 1)].message, 256, "%s", msg);

    atomic_store_explicit(&rb->head, curr_head + 1, memory_order_release);
    return 0;
}

static int ring_buffer_pop(log_ring_buffer_t *rb, log_item_t *item) {
    uint32_t curr_tail = atomic_load_explicit(&rb->tail, memory_order_relaxed);
    uint32_t curr_head = atomic_load_explicit(&rb->head, memory_order_acquire);

    if ((curr_head - curr_tail) > BUFFER_SIZE) {
        //drop message, buffer now is full (read is not catch up with write)
        return -1;
    }

    *item = rb->buffer[curr_tail & (BUFFER_SIZE-1)];
    atomic_store_explicit(&rb->tail, curr_tail+1, memory_order_release);
    return 0;
}