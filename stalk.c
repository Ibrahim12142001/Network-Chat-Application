#include <stdio.h>
#include "threads.h"
#include <unistd.h>





int main(int argc, char** argv) {
    initialize_chat(argc, argv);

    pthread_mutex_lock(&termination_mutex);
    if (!get_terminate_flag()) {
        pthread_cond_wait(&termination_cond, &termination_mutex);
    }
    pthread_mutex_unlock(&termination_mutex);

    terminate_chat();
    return 0;
}
