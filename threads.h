#ifndef _THREADS_H_
#define _THREADS_H_
#include "list.h"
#include <stdbool.h>
#define MAX_LEN 1024
#include <pthread.h>

// Struct to hold chat configuration
typedef struct {
    int local_port;
    char* remote_host;
    char* remote_port;
    int network_socket;
    struct addrinfo* remote_info;
    List* incoming_messages;
    List* outgoing_messages;
} ChatConfig;

ChatConfig chat_config;

extern pthread_mutex_t termination_mutex;
extern pthread_cond_t termination_cond;

// Thread functions
void* read_input(void* arg);
void* display_messages(void* arg);
void* receive_messages(void* arg);
void* send_messages(void* arg);
bool is_empty_string(const char* str);
void terminate_chat() ;
void initialize_chat(int argc, char** argv);
void set_terminate_flag();
int get_terminate_flag();

void sleep_usec(long usec);
void long_msec(long msec);

#endif