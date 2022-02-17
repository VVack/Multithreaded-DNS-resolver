#include "multi-lookup.h"
#include "fArray.h"
#include "array.h"

struct args {
    fArray fileArray;
    stack s;
};

void * requester(void *ptr) {

    FILE * fPointer;
    FILE * logPointer;
    char * filename;
    char * line = NULL;
    size_t length;                                                                                  //getline inputs
    ssize_t lineVal;
    int file_count = 0;

    struct args* value = ( struct args *)ptr;
    while (1) {
        pthread_mutex_lock(&(value->fileArray.input_files_mutex));                                  //critical section
        //printf("debug-input_files_mutex locked: %p\n",&(value->fileArray.input_files_mutex));

        if (value->fileArray.index == value->fileArray.size) {                                      //exit condition
            pthread_mutex_unlock(&(value->fileArray.input_files_mutex));
            //printf("debug-input_files_mutex unlocked: %p\n",&(value->fileArray.input_files_mutex));
            pthread_mutex_lock(&(value->s.exit_mutex));
            //printf("debug-exit_mutex locked: %p\n",&(value->s.exit_mutex));

            fprintf(stdout, "thread %lu serviced %d files\n", pthread_self(), file_count); 
            fflush(stdout);
            value->s.exit_count++; 
            pthread_mutex_unlock(&(value->s.exit_mutex));
            //printf("debug-exit_mutex unlocked: %p\n",&(value->s.exit_mutex));

            if (line) {
                free(line);
            }

            pthread_exit(0);
        }

        filename = value->fileArray.input_files[value->fileArray.index];

        value->fileArray.index++;

        pthread_mutex_unlock(&(value->fileArray.input_files_mutex));                                //end critical section
        //printf("debug-input_files_mutex unlocked: %p\n",&(value->fileArray.input_files_mutex));


        fPointer = fopen(filename, "r");

        while ( (lineVal = getline(&line,&length, fPointer)) != -1) {                               //getline returns -1 at EOF
            if (line[lineVal -1] == '\n') {
                line[lineVal -1] = '\0';
            }
            stackPush(&(value->s), line);                                                           

            logPointer = fopen(value->fileArray.req_log_file, "a");                                 

            pthread_mutex_lock(&(value->fileArray.req_log_mutex));                                  //critical section
            fprintf(logPointer, "%s\n", line);
            pthread_mutex_unlock(&(value->fileArray.req_log_mutex));                                //end critical section

            fclose(logPointer);


        
        }
        file_count++;
        fclose(fPointer);

    }

}

void *resolver(void *ptr)
{
    FILE *logPointer;   
    int count = 0;
    char name[MAX_NAME_LENGTH];
    char IPstring[MAX_IP_LENGTH] = "NOT";

    struct args* value = (struct args *)ptr;

    while (1)   {
        pthread_mutex_lock(&(value->s.exit_mutex)); 
        //printf("debug-exit_mutex locked: %p\n",&(value->s.exit_mutex));

        if (value->s.exit_count == value->s.res_count)   {                                          //exit condition: if all the requestor threads have left
        
            pthread_mutex_unlock(&(value->s.exit_mutex));
            //printf("debug-exit_mutex unlocked: %p\n",&(value->s.exit_mutex));


            pthread_mutex_lock(&(value->s.arr_mutex));
            //printf("debug-arr_mutex locked: %p\n",&(value->s.arr_mutex));

            if (value->s.top == 0)  {
                pthread_mutex_unlock(&(value->s.arr_mutex));
                fprintf(stdout, "thread %lu resolved %d hostnames\n", pthread_self(), count);
                fflush(stdout);
                sem_post(&(value->s.items));                                                        
                pthread_exit(0);
            }
            pthread_mutex_unlock(&(value->s.arr_mutex));
            //printf("debug-arr_mutex unlocked: %p\n",&(value->s.arr_mutex));

        }
        pthread_mutex_unlock(&(value->s.exit_mutex));
        //printf("debug-exit_mutex unlocked: %p\n",&(value->s.exit_mutex));
        stackPop(&(value->s), name); 

        if (dnslookup(name, IPstring, 46) == UTIL_FAILURE) {
            strncpy(IPstring,"NOT RESOLVED",MAX_IP_LENGTH);
        }
        else {
            count++;
        } 

        logPointer= fopen(value->fileArray.res_log_file, "a");                                      
        pthread_mutex_lock(&(value->fileArray.res_log_mutex));
        fprintf(logPointer, "%s, %s\n", name, IPstring);
        pthread_mutex_unlock(&(value->fileArray.res_log_mutex));
        fclose(logPointer);

    }
}



int main(int argc, char **argv) {
    
    if (argc < 6) {
        ERROR2("Incorrect Number of Arguments");
    }

    
    int valid_count = 0;
    int files[argc - 5];                                                                                
    struct args pass_vals;
    struct timeval start;
    struct timeval end;
    int file;
    char *ptr;
    int req_count;
    int res_count;

    gettimeofday(&start,NULL);

    req_count = strtol(argv[1], &ptr, 10);                                                                                  //input error checking
    if (*ptr != '\0') {
        ERROR1("Req Count must be a number");
    }
    res_count = strtol(argv[2],&ptr,10);
    if (*ptr != '\0') {
        ERROR1("Res Count must be a number");
    }
    if ((file = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC,0666)) == -1) {
        ERROR1("Request Log File Not Valid");
    }
    close(file);
    if ((file = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC,0666)) == -1) {
        ERROR1("Resolve Log File Not Valid");
    }
    close(file);
    if (req_count <= 0 || res_count <= 0) {
        ERROR1("Need Number of threads between 1-10");
    }
    if (req_count > (int)MAX_REQUESTER || res_count > (int)MAX_RESOLVER) {
        ERROR1("Need Number of threads between 1-10");
    }
    close(file);
    if ((argc - 5) > (int)MAX_INPUT_FILES){
        ERROR1("To Many Input Files");
    }

    pthread_t req_main[req_count];
    pthread_t res_main[res_count]; 

    for (int i = 5; i < argc; i++) {                                                                                        //checking for valid files
    
        if ((file = open(argv[i], O_RDONLY,0666)) == -1) {
            fprintf(stderr, "Invalid Input File %s\n", argv[i]);                                                            //check for invalid file names
        }
        else {
            valid_count++; 
            files[valid_count -1] = i;                                                       
        }
        close(file);
    }
    if ( valid_count == 0){
        ERROR1("Error Allocating Input Files");
    }
    fArrayInit(&(pass_vals.fileArray), argv, files, valid_count);
    stackInit(&(pass_vals.s), req_count); 

    for (int i = 0; i < req_count; i++) {
        pthread_create(&req_main[i], NULL, requester, &pass_vals);
    }

    for (int i = 0; i < res_count; i++) {
        pthread_create(&res_main[i], NULL, resolver, &pass_vals);
    }

    //Join all the threads
    for (int i = 0; i < req_count; i++) {
        pthread_join(req_main[i], NULL);
    }

    for (int i = 0; i < res_count; i++) {
        pthread_join(res_main[i], NULL);
    }

    stackFree(&(pass_vals.s)); 
    fArrayFree(&(pass_vals.fileArray)); 

    gettimeofday(&end,NULL);

    fprintf(stdout,"%s: total time is %.5f seconds\n", argv[0], (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)*1e-6);
    return 0;
}
