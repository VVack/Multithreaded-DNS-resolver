#include "fArray.h"

int fArrayInit(fArray *value, char **argv, int files[], int count) {
    value->index = 0;
    value->size = count;
    value->req_log_file = argv[3];
    value->res_log_file = argv[4];

    pthread_mutex_init(&(value->input_files_mutex), NULL);
    pthread_mutex_init(&(value->req_log_mutex), NULL);
    pthread_mutex_init(&(value->res_log_mutex), NULL);
    value->input_files = malloc(sizeof(char *)* count);


    for (int i =0; i < count; i++) {
        value->input_files[i] = argv[files[i]];
    }


    return 0;
}

int fArrayFree(fArray * value) {
    pthread_mutex_destroy(&(value->input_files_mutex));
    pthread_mutex_destroy(&(value->req_log_mutex));
    pthread_mutex_destroy(&(value->res_log_mutex));

    free(value->input_files);
    return 0;
}
