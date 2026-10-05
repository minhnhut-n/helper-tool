/**
 * Logging service stress free for Linux
 * Author: minhnhut-n
 * Date: 30/09/2026
 */
#pragma once
#include <stdint.h>

#ifdef __cplusplus
#include <atomic>
#define LOGGER_ATOMIC(type) std::atomic<type>
extern "C" {
#else
#include <stdatomic.h>
#define LOGGER_ATOMIC(type) _Atomic(type)
#endif
/////////
#ifndef LOGGER_COMMON_H
#define LOGGER_COMMON_H
#define RING_BUFFER_SZ 1024
#define MAX_LOG_LEN    128
#define LOG_FILE_PATH  "/tmp/logging.log"
#define LOG_FILE_SIZE  (1024u * 1024u)

typedef enum {
    LOG_INFO,
    LOG_DEBUG
} log_level_t;

//data struct for a log entry
typedef struct {
    LOGGER_ATOMIC(uint32_t) committed; // 0 = đang ghi dở, 1 = đã publish
    uint64_t timestamp;
    uint8_t level;
    char msg[MAX_LOG_LEN];
} log_entry_t;

//antomic data struct ring buffer
typedef struct {
    log_entry_t entry[RING_BUFFER_SZ];
    LOGGER_ATOMIC(uint32_t) head;
    LOGGER_ATOMIC(uint32_t) tail;
} atomic_ring_buffer_t;

//method
int log_enqueue(uint8_t level, const char* msg);

#endif
////////
#ifdef __cplusplus
}
#endif

#undef LOGGER_ATOMIC