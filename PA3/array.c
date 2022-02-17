#include "array.h"

int stackInit(stack * s, int res) {
  s->top = 0;
  sem_init(&(s->space),0,10);
  sem_init(&(s->items),0,0);
  pthread_mutex_init(&(s->arr_mutex), 0);
  /*for (int j = 0; j<10; j++) {
    s->arr[j] = malloc(sizeof(MAX_NAME_LENGTH));
  }
  */
  pthread_mutex_init(&(s->exit_mutex),0);
  s->exit_count = 0;
  s->res_count = res;
  return 0;
}

int stackFree(stack * s) {
  sem_destroy(&(s->space));
  sem_destroy(&(s->items));
  pthread_mutex_destroy(&(s->arr_mutex));
  pthread_mutex_destroy(&(s->exit_mutex));
  //free(s);
  return 0;
}

int stackPop(stack * s, char * e) {
  //printf("in stack pop \n");
  sem_wait(&(s->items));
  pthread_mutex_lock(&(s->arr_mutex));
  //printf("debug-arr_mutex locked: %p\n",&(s->arr_mutex) );
  //printf("past mutex \n");

  if (s->top == 0) {
    pthread_mutex_unlock(&(s->arr_mutex));
    sem_post(&(s->items));
    pthread_exit(0);
  }

  memset(e,'\0',MAX_NAME_LENGTH);
  s->top = s->top - 1;
  //printf("past top set \n");
  strncpy(e, (char *)(s->arr[s->top]), MAX_NAME_LENGTH);
  //printf("past strncpy \n");
  //printf("popped: %c \n", *(s->arr[s->top]));

  pthread_mutex_unlock(&(s->arr_mutex));
  //printf("debug-arr_mutex unlocked: %p\n",&(s->arr_mutex) );
  sem_post(&(s->space));
  return 0;
}

int stackPush(stack * s, char * e) {
  //printf("in stack push \n");
  sem_wait(&(s->space));
  pthread_mutex_lock(&(s->arr_mutex));
  //printf("debug-arr_mutex locked: %p\n",&(s->arr_mutex) );


  strncpy((char*) (s->arr[s->top]), e, MAX_NAME_LENGTH);
  //printf("after strncpy \n");
  s->top = s->top + 1;
  //printf("pushed: %c \n", *(s->arr[s->top -1]));

  pthread_mutex_unlock(&(s->arr_mutex));
  //printf("debug-arr_mutex unlocked: %p\n",&(s->arr_mutex) );

  sem_post(&(s->items));
  return 0;

}

int stackPrint(stack * s) {
  for (int i = 0; i < s->top; i++) {
    printf("%s", *(s->arr[i]));
  }
  printf("\n");
  return 0;
}
