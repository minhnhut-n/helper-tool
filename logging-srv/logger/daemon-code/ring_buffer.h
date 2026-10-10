#ifndef _RING_BUFFER_
#define _RING_BUFFER_

#include <stdatomic.h>
#include <string.h>

typedef struct {
    log_type_t level;
    const char* msg;
} log_item_t;

//ring buffer
typedef struct {
    _Atomic((uint32_t)) head;
    _Atomic((uint32_t)) tail;
    log_item_t buffer[BUFFER_SIZE];
} log_ring_buffer_t;

static int ring_buffer_push(log_ring_buffer_t *rb, int level, const char *msg);
static int ring_buffer_pop(log_ring_buffer_t *rb);

#endif