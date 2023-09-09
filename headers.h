#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>


#define SEM_NAME1 "file_req"
#define SEM_NAME2 "wake_up_parent"
#define SEM_NAME3 "answer"

#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1
#define INITIAL_VALUE_WAKE 0

struct temp_struct {
	int line_start;
    int line_end;
	char line[2048];
};

struct shared_struct {
	int file_num;
    int line_start;
    int line_end;
    char name_of_file[2048];
    int temp_id;
    char name_of_semaphore[2048];
    char name_of_semaphore_ans[2048];
};

struct args {
    char name_of_file[2048];
    int temp_id;
    char name_of_semaphore[2048];
    char name_of_semaphore_ans[2048];
    int line_start;
    int line_end;
};

#define MAX_LINES 10

void work(int numOfFilesWanted,sem_t* semaphore_req,int i,struct shared_struct * shared,sem_t* semaphore_wake,sem_t* semaphore_transactions,sem_t* semaphore_transactions_ans,sem_t* semaphore_answer,int lambda);
void* threadWorker(void* args);