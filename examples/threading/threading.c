#include "threading.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Optional: use these functions to add debug or error prints to your
// application
#define DEBUG_LOG(msg, ...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg, ...) printf("threading ERROR: " msg "\n", ##__VA_ARGS__)

static int msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

void* threadfunc(void* thread_param) {
    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data
    // structure hint: use a cast like the one below to obtain thread arguments
    // from your parameter
    struct thread_data* thread_func_args = (struct thread_data*)thread_param;

    msleep(thread_func_args->wait_to_obtain_ms);

    pthread_mutex_lock(thread_func_args->mutex);

    msleep(thread_func_args->wait_to_release_ms);

    pthread_mutex_unlock(thread_func_args->mutex);

    thread_func_args->thread_complete_success = true;

    return thread_func_args;
}

bool start_thread_obtaining_mutex(pthread_t* thread, pthread_mutex_t* mutex,
                                  int wait_to_obtain_ms,
                                  int wait_to_release_ms) {
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments,
     * pass thread_data to created thread using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    struct thread_data* pThreadData =
        (struct thread_data*)malloc(sizeof(struct thread_data));
    if (pThreadData == NULL) {
        return false;
    }

    pThreadData->wait_to_obtain_ms = wait_to_obtain_ms;
    pThreadData->wait_to_release_ms = wait_to_release_ms;
    pThreadData->mutex = mutex;
    pThreadData->thread = thread;
    pThreadData->thread_complete_success = false;

    if (pthread_create(thread, NULL, threadfunc, pThreadData) != 0) {
        perror("pthread_create() error");
        return false;
    }

    return true;
}
