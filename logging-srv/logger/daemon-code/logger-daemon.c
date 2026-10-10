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

static log_ring_buffer_t g_ringbuf = {.head=0, .tail=0};

//background worker
void* worker_thread(void *arg) {
    log_item_t item;
    while(1) {
        if (ring_buffer_pop(&g_ringbuf, &item) < 0) {
            usleep(1000); // avoid overhead CPU
            continue;
        }
        //action we want to work with this log, temporary, just print it out
        //to journalctl        
        printf("Daemon process: %d, %s", item.level, item.msg);
        fflush(stdout); //function to push infomation to "stdout"     
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
    while(true) {
        //only accept file descriptor, dont case other things else
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            continue;
        }

        char buffer[512];
        ssize_t socket_data_size;
        while ((socket_data_size = read(client_fd, buffer, sizeof(buffer)-1)) > 0) (
            //raw byte read return
            buffer[n] = '\0'; // add endline
            int level = LOG_DEBUG;
            char msg[256];
            //sscanf return the number successfully assign for with success case.
            if (sscanf(buffer, "%d: %255[^\n]", &level, &msg) == 2) {
                ring_buffer_push(&g_ringbuf, level, msg);
            }
        )
        //client done -> close()
        close(client_fd);
    }
    //not expect jump to this case, only when system shut down
    close(server_fd);
    unlink(SOCK_PATH);
    return 0;
}

