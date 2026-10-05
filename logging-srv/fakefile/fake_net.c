#include "logging-linux-srv.h"

// Fake module: network — file .c thứ 2 cùng include service logger
void fake_net_dump(int count);

#include <stdio.h>

void fake_net_dump(int count) {
    char buf[MAX_LOG_LEN];
    for (int i = 0; i < count; i++) {
        snprintf(buf, sizeof(buf), "[net] rx_pkt=%d len=%d bytes", i, 64 + (i * 7) % 1400);
        log_level_t lv = (i % 3 == 0) ? LOG_DEBUG : LOG_INFO;
        if (log_enqueue(lv, buf) != 0) {
            fprintf(stderr, "net: ring full, drop %d\n", i);
        }
    }
}
