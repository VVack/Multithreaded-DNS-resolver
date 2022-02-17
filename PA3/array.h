#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define ARRAY_SIZE 10
#define MAX_NAME_LENGTH 255

typedef struct {
  char * arr[ARRAY_SIZE][MAX_NAME_LENGTH];
  int top;
  pthread_mutex_t arr_mutex;
  sem_t space;
  sem_t items;

  int exit_count;
  int res_count;
  pthread_mutex_t exit_mutex;
}stack;

struct params{
  stack * stk;
  char * e;
};

int stackInit(stack *s, int res);
int stackFree(stack *s);
int stackPush(stack *s, char * e);
int stackPop(stack *s, char * e);
int stackPrint(stack * s);

#endif
