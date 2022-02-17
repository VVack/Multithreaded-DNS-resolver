#include "array.h"
#include <stdlib.h>
#include <stdio.h>

void * producer (void * ptr) {
  //printf("In Producer\n");
  char test = 'x';
  stack * stk = (stack *) ptr;
  //printf("before push\n");
  for (int i = 0; i<5; i++) {
    stackPush(stk, &test);
  }
  pthread_exit(0);
}

void * producer2 (void * ptr) {
  //printf("In Producer 2\n");
  char test = 'o';
  stack * stk = (stack *) ptr;
  //printf("before push\n");
  for (int i = 0; i<5; i++) {
    stackPush(stk, &test);
  }
  pthread_exit(0);
}

void * producer3 (void * ptr) {
  //printf("In Producer 2\n");
  char test = 'z';
  stack * stk = (stack *) ptr;
  //printf("before push\n");
  for (int i = 0; i<5; i++) {
    stackPush(stk, &test);
  }
  pthread_exit(0);
}

void * consumer (void * ptr) {
  //printf("In Consumer\n");
  char test2[MAX_NAME_LENGTH];
  stack * stk = (stack *) ptr;
  //printf("before pop\n");
  for (int i = 0; i<5; i++) {
    stackPop(stk, test2);
  }
  //printf("element popped %c \n", *test2);
  pthread_exit(0);
}

int main() {
    stack my_stack;
    int i;
    void * param = &my_stack;

    if (stackInit(&my_stack) < 0) {
      printf("init failed");
    }

    pthread_t prod;
    pthread_t prod2;
    //pthread_t prod3;
    pthread_t cons;
    pthread_t cons2;
    printf("before pthread create \n");
    pthread_create(&prod, NULL, producer, param);
    pthread_create(&prod2, NULL, producer2, param);
    //pthread_create(&prod3, NULL, producer3, param);
    pthread_create(&cons, NULL, consumer, param);
    pthread_create(&cons2, NULL, consumer, param);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    pthread_join(prod2, NULL);
    pthread_join(cons2, NULL);
    //pthread_join(prod3, NULL);
    stackPrint(param);
}
