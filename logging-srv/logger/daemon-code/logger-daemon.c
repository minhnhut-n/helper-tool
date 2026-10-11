#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdatomic.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "ring_buffer.h"
#define SOCK_PATH "/tmp/logger.sock"
#define FILE_PATH "/home/aelius/envDev/logger.log"

static log_ring_buffer_t g_ringbuf = {.head=0, .tail=0};

//background worker
void* worker_thread(void *arg) {
    (void) arg;
    log_item_t item;
    while(1) {
        if (ring_buffer_pop(&g_ringbuf, &item) < 0) {
            usleep(1000); // avoid overhead CPU
            continue;
        }
        //action we want to work with this log, temporary, just print it out
        //to journalctl        
        // printf("Daemon process: %d, %s\n", item.level, item.msg);
        // fflush(stdout); //function to push infomation to "stdout" 
        
        FILE *file = fopen(FILE_PATH, "a");
        if (file != NULL) {
            fprintf(file, "%d : %s\n", item.level, item.msg);
            fclose(file);
        }
        else {
            printf("can not create file log\n");
        }
    }
    //not expect jump to this line
    return NULL;
}


//main logic
int main(void) {
    // only run one time
    unlink(SOCK_PATH); // from previous dead service,... handle bug

    //kernel socket create
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket error");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path)-1);

    // attach socket with local IP and port
    // bind() --> listen()
    /** technically we can type cast from (sockaddr_un) --> (sockaddr)
     * because then have same meaning in struct memory layout, declaration
     * (we can think as _in/_un is derivative class from original one)
     * 
     * but it have limited size in address of sockaddr (pay attention to it)
     */
    if(bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0 || 
        listen(server_fd, 5) < 0) {
            perror("Bind/Listen error");
            close(server_fd);
            return -1;
        }

    //background worker call
    pthread_t worker;
    pthread_create(&worker, NULL, worker_thread, NULL);

    // in socket tcp server, we need to open a listen for client search
    /**
     * the process is 
     * socket() -> (optional) -> bind() -> listen() -> accept() -> read()/write()
     * close() --- this is when system shutdown.
     */
    while(1) {
        //only accept file descriptor, dont case other things else
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            continue;
        }

        char buffer[512];
        char line[512];
        size_t line_length = 0;
        int discarding_line = 0;
        ssize_t socket_data_size;
        while ((socket_data_size = read(client_fd, buffer, sizeof(buffer))) > 0) {
            for (ssize_t i = 0; i < socket_data_size; i++) {
                if (buffer[i] == '\n') {
                    if (!discarding_line) {
                        line[line_length] = '\0';
                        int level = LOG_DEBUG;
                        char msg[256];
                        if (sscanf(line, "%d: %255[^\r\n]", &level, msg) == 2) {
                            ring_buffer_push(&g_ringbuf, level, msg);
                        }
                    }
                    line_length = 0;
                    discarding_line = 0;
                } else if (!discarding_line) {
                    if (line_length < sizeof(line) - 1) {
                        line[line_length++] = buffer[i];
                    } else {
                        fprintf(stderr, "Discarding oversized log message\n");
                        line_length = 0;
                        discarding_line = 1;
                    }
                }
            }
        }
        //client done -> close()
        close(client_fd);
    }
    //not expect jump to this case, only when system shut down
    close(server_fd);
    unlink(SOCK_PATH);
    return 0;
}
