#ifndef FARRAY_H
#define FARRAY_H

#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct {
    char **input_files;
    int index;
    int size;
    char *req_log_file;
    char *res_log_file;
    pthread_mutex_t input_files_mutex;
    pthread_mutex_t req_log_mutex;
    pthread_mutex_t res_log_mutex;
}fArray;

int fArrayInit(fArray * value, char **argv, int files[], int count);
int fArrayFree(fArray * value);

#endif
