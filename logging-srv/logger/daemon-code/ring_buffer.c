#include "ring_buffer.h"

#include <stdio.h>

int ring_buffer_push(log_ring_buffer_t *rb, int level, const char *msg) {
    uint32_t curr_head, curr_tail;
    
    //CAS standard
    do {
        curr_head = atomic_load_explicit(&rb->head, memory_order_relaxed);
        curr_tail = atomic_load_explicit(&rb->tail, memory_order_acquire);

        if ((curr_head - curr_tail) >= BUFFER_SIZE) {
            //drop message, buffer now is full (read is not catch up with write)
            return -1;
        }

    } while (!atomic_compare_exchange_weak_explicit(&rb->head,              //the addr need to be made change
                                                    &curr_head,             //the expected in RAM
                                                    curr_head + 1,          //the new desired value, want to be overrided (if fail)
                                                    memory_order_acquire,   //the order if success
                                                    memory_order_relaxed)); //order if fail

    //assign message struct to buffer
    int write_idx = curr_head & (BUFFER_SIZE - 1);
    rb->buffer[write_idx].level = level;
    snprintf(rb->buffer[write_idx].msg, LOG_MESSAGE_SIZE, "%s", msg);

    // mark this index is done, reader can read
    atomic_store_explicit(&rb->buffer[write_idx].committed, 1, memory_order_release);
    return 0;
}

int ring_buffer_pop(log_ring_buffer_t *rb, log_item_t *item) {
    uint32_t curr_head, curr_tail;
    do {
        curr_tail = atomic_load_explicit(&rb->tail, memory_order_relaxed);
        curr_head = atomic_load_explicit(&rb->head, memory_order_acquire);

        if (curr_head == curr_tail) {
            //empty buffer -> stop read -> release cpu
            return -1;
        }

        int read_idx = curr_tail & (BUFFER_SIZE-1);
        if (atomic_load_explicit(&rb->buffer[read_idx].committed, memory_order_acquire) == 0) {
            return -1; // on performing write.
        }

    } while (!atomic_compare_exchange_weak_explicit(&rb->tail,
                                                    &curr_tail,
                                                    curr_tail + 1,
                                                    memory_order_acquire,
                                                    memory_order_relaxed));

    // slot succcess, re-read the idx
    int read_idx = curr_tail & (BUFFER_SIZE - 1);
    *item = rb->buffer[read_idx];

    // reset committed slot, ready for next write.
    atomic_store_explicit(&rb->buffer[read_idx].committed, 0, memory_order_release);
    return 0;
}