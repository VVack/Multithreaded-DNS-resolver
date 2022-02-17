#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "util.h"
#include "fArray.h"
#include "array.h"

#define ARRAY_SIZE 10
#define MAX_INPUT_FILES 100
#define MAX_REQUESTER 10
#define MAX_RESOLVER 10
#define MAX_NAME_LENGTH 255
#define MAX_IP_LENGTH INET6_ADDRSTRLEN

#define ERROR1(S) { fprintf(stderr, "%s error %s\n", argv[0], S); exit(-1);}
#define ERROR2(S) { fprintf(stdout, "%s error %s\n", argv[0], S); exit(-1);}

void *resolver(void *ptr);
void *requester(void *ptr);



#endif
