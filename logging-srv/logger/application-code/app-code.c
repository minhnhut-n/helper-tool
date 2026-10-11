#include <stdio.h>
#include <pthread.h>
#include "logger-srv.h"

#define SOCK_PATH "/tmp/logger.sock"

void *doJob1(void *arg) {
    int i = 0;
    while (i<4) {
        logger(LOG_DEBUG, "nhut job1");
    }
}
void *doJob2(void *arg) {
    int i = 0;
    while (i<5) {
        logger(LOG_DEBUG, "nhut job2");
    }
}
void *doJob3(void *arg) {
    int i = 0;
    while (i<6) {
        logger(LOG_DEBUG, "nhut job3");
    }
}

int main(void) {
    pthread_t threadID1, threadID2, threadID3;
    int threadArgs = 12;
    pthread_create(&threadID1, NULL, doJob1, &threadArgs);
    pthread_create(&threadID2, NULL, doJob2, &threadArgs);
    pthread_create(&threadID3, NULL, doJob3, &threadArgs);
    logger_init(SOCK_PATH);
    logger(LOG_INFO, "hello world!");
    logger_close();
    return 0;
}