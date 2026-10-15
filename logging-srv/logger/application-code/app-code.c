#include <stdio.h>
#include <pthread.h>
#include "logger-srv.h"

#define SOCK_PATH "/tmp/logger.sock"

void *doJob1(void *arg) {
    (void)arg;
    int i = 0;
    while (i<4) {
        logger(LOG_FORCE, "nhut job1, %d", i);
        i++;
    }
    return NULL;
}
void *doJob2(void *arg) {
    (void)arg;
    int i = 0;
    while (i<5) {
        logger(LOG_INFO, "nhut job2, %d", i);
        i++;
    }
    return NULL;
}
void *doJob3(void *arg) {
    (void)arg;
    int i = 0;
    while (i<6) {
        logger(LOG_DEBUG, "nhut job3, %d", i);
        i++;
    }
    return NULL;
}

int main(void) {
    if (logger_init(SOCK_PATH) < 0) {
        perror("logger_init");
        return 1;
    }

    pthread_t threadID1, threadID2, threadID3;
    int threadArgs = 12;

    if (pthread_create(&threadID1, NULL, doJob1, &threadArgs) != 0 ||
        pthread_create(&threadID2, NULL, doJob2, &threadArgs) != 0 ||
        pthread_create(&threadID3, NULL, doJob3, &threadArgs) != 0) {
        fprintf(stderr, "Unable to create logger worker thread\n");
        logger_close();
        return 1;
    }

    pthread_join(threadID1, NULL);
    pthread_join(threadID2, NULL);
    pthread_join(threadID3, NULL);
    logger(LOG_INFO, "hello world!");
    logger_close();
    return 0;
}