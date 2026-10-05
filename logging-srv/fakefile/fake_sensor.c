#include "logging-linux-srv.h"

// Fake module: cảm biến nhiệt độ — chứng minh file .c riêng vẫn include được logger
void fake_sensor_dump(int count);

#include <stdio.h>

void fake_sensor_dump(int count) {
    char buf[MAX_LOG_LEN];
    for (int i = 0; i < count; i++) {
        // timestamp do log_enqueue() tự lấy bằng time(NULL), mình chỉ cần snprintf msg
        snprintf(buf, sizeof(buf), "[sensor] temp=%d.%dC sample=%d", 36 + (i % 5), i % 10, i);
        if (log_enqueue(LOG_INFO, buf) != 0) {
            fprintf(stderr, "sensor: ring full, drop %d\n", i);
        }
    }
}
