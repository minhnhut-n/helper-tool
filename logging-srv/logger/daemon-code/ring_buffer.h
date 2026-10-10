#ifndef _RING_BUFFER_
#define _RING_BUFFER_

#include <stdint.h>
#include <stdatomic.h>

#define BUFFER_SIZE 1024
#define LOG_MESSAGE_SIZE 256

typedef enum {
    LOG_INFO,
    LOG_DEBUG,
    LOG_ERROR,
    LOG_FORCE
} log_type_t;

typedef struct {
    log_type_t level;
    char msg[LOG_MESSAGE_SIZE];
} log_item_t;

//ring buffer
typedef struct {
    _Atomic(uint32_t) head;
    _Atomic(uint32_t) tail;
    log_item_t buffer[BUFFER_SIZE];
} log_ring_buffer_t;

int ring_buffer_push(log_ring_buffer_t *rb, int level, const char *msg);
int ring_buffer_pop(log_ring_buffer_t *rb, log_item_t *item);

#endif