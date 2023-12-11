#include "threads.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>
#include "list.h"
#include <unistd.h>   
#include <ctype.h>    
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>


#define TERMINATE_CHAT_SIGNAL "!"

static pthread_mutex_t incoming_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t outgoing_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t incoming_cond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t outgoing_cond = PTHREAD_COND_INITIALIZER;
static pthread_t input_thread, display_thread, receive_thread, send_thread;

static volatile int terminate_flag = 0; 
pthread_mutex_t termination_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t termination_cond = PTHREAD_COND_INITIALIZER;

void sleep_usec(long usec){

    struct timespec sleep_time;
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = usec * 1000;
    nanosleep(&sleep_time, NULL);

}
void sleep_msec(long msec){
    sleep_usec(msec * 1000);
}

void initialize_chat(int argc, char** argv) {
    if(argc != 4) {
        fprintf(stderr, "Please enter in the following format: s-talk [local port] [remote host] [remote port]\n");
        exit(EXIT_FAILURE);
    }
    char* msg = "\033[92mSetting up a connection Sire! Please wait.\n"; 
    for (int i = 0; i < strlen(msg); i++){
        printf("%c", msg[i]);
        fflush(stdout);
        sleep_msec(50);
    }

    char* msg3 = ".....\n"; 
    for (int k = 0; k < strlen(msg3); k++){
        printf("%c", msg3[k]);
        fflush(stdout);
        sleep_msec(500);
    }
  
    
    chat_config.local_port = atoi(argv[1]);
    chat_config.remote_host = argv[2];
    chat_config.remote_port = argv[3];
    chat_config.incoming_messages = List_create();
    chat_config.outgoing_messages = List_create();

    struct sockaddr_in local_address;
    struct addrinfo hints;

    memset(&local_address, 0, sizeof(local_address));
    memset(&hints, 0, sizeof(hints));

    local_address.sin_family = AF_INET;
    local_address.sin_addr.s_addr = htonl(INADDR_ANY);
    local_address.sin_port = htons(chat_config.local_port);
    
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if(getaddrinfo(chat_config.remote_host, chat_config.remote_port, &hints, &chat_config.remote_info) != 0) {
        perror("Error getting remote address info");
        exit(EXIT_FAILURE);
    }

    chat_config.network_socket = socket(PF_INET, SOCK_DGRAM, 0);
    if(chat_config.network_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    if(bind(chat_config.network_socket, (struct sockaddr*)&local_address, sizeof(local_address)) == -1) {
        perror("Error binding socket");
        close(chat_config.network_socket);
        exit(EXIT_FAILURE);
    }
    char* msg2 = "We are set up Sire! Ready to talk. Please enter \033[91m'!'\033[92m to terminate the chat.\033[94m\n";
    for (int j = 0; j < strlen(msg2); j++){
        printf("%c", msg2[j]);
        fflush(stdout);
        sleep_msec(30);
    }

    pthread_create(&input_thread, NULL, read_input, NULL);
    pthread_create(&display_thread, NULL, display_messages, NULL);
    pthread_create(&receive_thread, NULL, receive_messages, NULL);
    pthread_create(&send_thread, NULL, send_messages, NULL);


}

void terminate_chat() {
    pthread_cancel(input_thread);
    pthread_cancel(display_thread);
    pthread_cancel(receive_thread);
    pthread_cancel(send_thread);
    pthread_join(input_thread, NULL);
    pthread_join(display_thread, NULL);
    pthread_join(receive_thread, NULL);
    pthread_join(send_thread, NULL);
    

    List_free(chat_config.incoming_messages, free);
    List_free(chat_config.outgoing_messages, free);
    freeaddrinfo(chat_config.remote_info);
    pthread_mutex_destroy(&incoming_mutex);
    pthread_mutex_destroy(&outgoing_mutex);
    pthread_cond_destroy(&incoming_cond);
    pthread_cond_destroy(&outgoing_cond);
    close(chat_config.network_socket);
    printf("\033[0m");


}

bool is_empty_string(const char* str) {
    while(*str) {
        if(!isspace((unsigned char)*str))
            return false;
        str++;
    }
    return true;
}

void* read_input(void* arg) {
    char buffer[MAX_LEN];
    fd_set read_fds;
    struct timeval timeout;

    while(terminate_flag == 0) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int ret = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

        if (ret > 0) { 
            fgets(buffer, MAX_LEN, stdin);
            if (strcmp(buffer, "!\n") == 0) {
                sendto(chat_config.network_socket, TERMINATE_CHAT_SIGNAL, strlen(TERMINATE_CHAT_SIGNAL), 0, chat_config.remote_info->ai_addr, chat_config.remote_info->ai_addrlen);
                set_terminate_flag();
                break;
            }
            if(!is_empty_string(buffer)) {
                pthread_mutex_lock(&outgoing_mutex);
                List_append(chat_config.outgoing_messages, strdup(buffer));
                pthread_cond_signal(&outgoing_cond);
                pthread_mutex_unlock(&outgoing_mutex);
            }
        } else if (ret == 0) { 
            continue;
        } else { 
            perror("tis not working");
            break;
        }
    }
    return NULL;
}


void* display_messages(void* arg) {
    while(terminate_flag == 0) {
        pthread_mutex_lock(&incoming_mutex);
        while(List_count(chat_config.incoming_messages) == 0) {
            pthread_cond_wait(&incoming_cond, &incoming_mutex);
        }
        char* message = List_trim(chat_config.incoming_messages);
        pthread_mutex_unlock(&incoming_mutex);

        if(message) {

            printf("\033[95mSire, a message from the other side: \033[94m%s", message);
            free(message);
        }
    }
    return NULL;
}

void* receive_messages(void* arg) {
    char buffer[MAX_LEN];
    while(terminate_flag == 0) {
        ssize_t bytes_received = recv(chat_config.network_socket, buffer, MAX_LEN, 0);
        if(bytes_received > 0) {
            buffer[bytes_received] = '\0';
            if (strcmp(buffer, "!") == 0) {
            char* msg = "\033[91mSire, the chat was terminated from the other side. Goodbye.\n"; 
                for (int i = 0; i < strlen(msg); i++){
                    printf("%c", msg[i]);
                    fflush(stdout);
                    sleep_msec(50);
                    }           
                    set_terminate_flag();
                break;
                
            }
            pthread_mutex_lock(&incoming_mutex);
            List_append(chat_config.incoming_messages, strdup(buffer));
            pthread_cond_signal(&incoming_cond);
            pthread_mutex_unlock(&incoming_mutex);
        }
    }
    return NULL;
}

void* send_messages(void* arg) {
    while(terminate_flag == 0) {
        pthread_mutex_lock(&outgoing_mutex);
        while(List_count(chat_config.outgoing_messages) == 0) {
            pthread_cond_wait(&outgoing_cond, &outgoing_mutex);
        }
        char* message = List_trim(chat_config.outgoing_messages);
        pthread_mutex_unlock(&outgoing_mutex);
       
        if(message) {
            sendto(chat_config.network_socket, message, strlen(message), 0, chat_config.remote_info->ai_addr, chat_config.remote_info->ai_addrlen);
            free(message);
        }
    }
    return NULL;
}

void set_terminate_flag() {
    pthread_mutex_lock(&termination_mutex);
    terminate_flag = 1;
    pthread_cond_signal(&termination_cond);
    pthread_mutex_unlock(&termination_mutex);
}

int get_terminate_flag() {
    return terminate_flag;
}
