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

#define SEM_NAME1 "file_req2"
#define SEM_NAME2 "wake_up_parent2"
#define SEM_NAME3 "answer2"

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

void* threadWorker(void* args){
    printf("THREAD\n");
    sem_t *semaphore = sem_open(SEM_NAME3, O_RDWR);
    if (semaphore == SEM_FAILED) {
        strerror("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    printf("hellooooo!\n");
    void *shared_temp = (void *)0;
	struct temp_struct *shared_t;
    struct args* args_ = (struct args*)args;
    int shmid = args_->temp_id;
    shared_temp = shmat(shmid, (void *)0, 0);
	if (shared_temp == (void *)-1) {
	 	fprintf(stderr, "shmat failed\n");
	 	exit(EXIT_FAILURE);
	}
	printf("Shared temp segment with id %d attached at %p\n", shmid, shared_temp);
    shared_t = (struct shared_temp *)shared_temp;
    char name_of_semaphore[2048];
    char name_of_semaphore_ans[2048];
    char name_of_temp[2048];
    strcpy(name_of_semaphore,args_->name_of_semaphore);
    strcpy(name_of_semaphore_ans,args_->name_of_semaphore_ans);
    strcpy(name_of_temp,args_->name_of_semaphore);
    sem_t * transactions_sem = sem_open(name_of_semaphore, O_RDWR);
    printf("name of semaphore %s\n",name_of_semaphore);
    if (transactions_sem == SEM_FAILED) {
        // strerror("sem_open(3) failed");
        // exit(EXIT_FAILURE);
        printf("SKATA\n");
    }
    sem_t * transactions_sem_ans = sem_open(name_of_semaphore_ans, O_RDWR);
    printf("name of semaphore answer %s\n",name_of_semaphore_ans);
    if (transactions_sem_ans == SEM_FAILED) {
        // strerror("sem_open(3) failed");
        // exit(EXIT_FAILURE);
        printf("SKATA2\n");
    }
    int line_start = args_->line_start;
    int line_end = args_->line_end;
    char filename[2048];
    sprintf(filename,"./test_folder/%s",args_->name_of_file);
    fp = fopen(filename, "r");
    if (fp == NULL)
        printf("didnt find file\n");
    int i = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        if(i>=line_start){
            printf("Retrieved line of length %zu:\n", read);
            printf("%s", line);
            sprintf(shared_t->line,line);
            // int value = 0;
            // int ret_val = sem_getvalue(transactions_sem,value);
            // printf("Semaphore before post has value:%d and ret_val:%d\n in thread",value,ret_val);
            if (sem_post(transactions_sem) < 0) {
                printf("sem_post(3) failed");
                continue;
            }
            // ret_val = sem_getvalue(transactions_sem,value);
            // printf("Semaphore after post has value:%d and ret_val:%d\n in thread",value,ret_val);
            printf("posted the sem %s waiting for responce\n",name_of_semaphore);
            if (sem_wait(transactions_sem_ans) < 0) {
                printf("sem_post(3) failed");
                continue;
            }
        }
        if(i >= line_end){
            break;
        }
        i++;
    }

    fclose(fp);
    if (line)
        free(line);
    printf("EXITED\n");
    // if (sem_post(semaphore) < 0) {
    //     strerror("sem_post(3) error on child");
    // }
    // printf("posted sem\n");
    pthread_exit((void*)EXIT_SUCCESS);
}

int expodential_distributed_number_generator(float lambda_parameter){
    double formal = rand() / (RAND_MAX + 1.0);
    int return_num = (int) (-log(1- formal) / lambda_parameter);
    return return_num;
}

int main(int argc,char** argv){
    sem_t *semaphore_req = sem_open(SEM_NAME1, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    if (semaphore_req == SEM_FAILED) {
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }
    sem_t *semaphore_wake = sem_open(SEM_NAME2, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE_WAKE);
    if (semaphore_wake == SEM_FAILED) {
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }
    sem_t *semaphore_answer = sem_open(SEM_NAME3, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE_WAKE);
    if (semaphore_answer == SEM_FAILED) {
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }
    void *shared_memory = (void *)0;
    int shmid = shmget((key_t)1234, sizeof(struct shared_struct), 0666 | IPC_CREAT);
	if (shmid == -1) {
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}
    shared_memory = shmat(shmid, (void *)0, 0);
	if (shared_memory == (void *)-1) {
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	//printf("Shared memory segment with id %d attached at %p\n", shmid, shared_memory);
    struct shared_struct * shared =  (struct shared_struct *) shared_memory;
    shared->file_num = 1;
    int numberOfProcesses = atoi(argv[1]);
    sem_t *semaphore_transactions[numberOfProcesses];
    sem_t *semaphore_transactions_ans[numberOfProcesses];
    pid_t pid[numberOfProcesses];
    int numOfFilesWanted = atoi(argv[2]);
    //printf("Argument is %d\n",numberOfProcesses);
    int i = 0;
    for(i = 0; i < numberOfProcesses; i++){
        char sem_name[1024];
        sprintf(sem_name, "TS%d", i);
        printf("NAME OF SEM%s\n",sem_name);
        semaphore_transactions[i] = sem_open(sem_name, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE_WAKE);
        if (semaphore_transactions[i] == SEM_FAILED) {
            perror("sem_open(3) error");
            exit(EXIT_FAILURE);
        }
    }
    for(i = 0; i < numberOfProcesses; i++){
        char sem_name[1024];
        sprintf(sem_name, "TS_ANS%d", i);
        printf("NAME OF SEM %s\n",sem_name);
        semaphore_transactions_ans[i] = sem_open(sem_name, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE_WAKE);
        if (semaphore_transactions_ans[i] == SEM_FAILED) {
            perror("sem_open(3) error");
            exit(EXIT_FAILURE);
        }
    }
    for(i = 0; i < numberOfProcesses; i++){
        pid[i] = fork();
        if(!pid[i]){
            break;
        }
    }
    printf("HELLO\n");
    if (pid[i] == 0){
        printf("Inside child\n");
        char temp_name[100];
        sprintf(temp_name, "%8d.log", getpid());
        FILE *fp;
        fp = fopen(temp_name, "w"); 
        key_t key;
        if ((key = ftok(temp_name, 'R')) == -1) {
		    perror("ftok");
		    exit(1);
	    }
        void *shared_temp = (void *)0;
	    struct temp_struct *shared_t;
        int shmid = shmget(key, sizeof(struct temp_struct), 0666 | IPC_CREAT);
	    if (shmid == -1) {
		    fprintf(stderr, "shmget failed\n");
		    exit(EXIT_FAILURE);
	    }
        shared_temp = shmat(shmid, (void *)0, 0);
	    if (shared_temp == (void *)-1) {
		    fprintf(stderr, "shmat failed\n");
		    exit(EXIT_FAILURE);
	    }
	    //printf("Shared temp segment with id %d attached at %p\n", shmid, shared_memory);
        shared_t = (struct shared_temp *)shared_temp;
        for(int k = 0;k < numOfFilesWanted;k++){
            struct dirent *de;
            // opendir() returns a pointer of DIR type. 
            DIR *dr = opendir("./test_folder");
            if (dr == NULL)  // opendir returns NULL if couldn't open directory
            {
                printf("Could not open current directory" );
                return 0;
            }
            srand(time(NULL));
            int randomNum = (rand() % 3) + 1;
            int it = 0;
            char name[2048];
            printf("Random number is %d\n",randomNum);
            while ((de = readdir(dr)) != NULL ){
                if(de->d_type == 8){
                    it++;
                    if(it == randomNum){
                        strcpy(name,de->d_name);
                    }
                }
            }
            if (sem_wait(semaphore_req) < 0) {
                perror("sem_wait(3) failed on child");
                continue;
            }
            strcpy(shared->name_of_file,name);
            printf("SHARED NAME IS %s\n",shared->name_of_file);
            // int value = 0;
            // int ret_val = sem_getvalue(semaphore_transactions[i],value);
            // printf("Semaphore before wait has value:%d and ret_val:%d\n in child",value,ret_val);
            shared->line_start = 0;
            shared->line_end = 0;
            shared->temp_id = shmid;
            char sem_name[1024];
            sprintf(sem_name, "TS%d", i);
            strcpy(shared->name_of_semaphore,sem_name);
            char sem_name_ans[1024];
            sprintf(sem_name_ans, "TS_ANS%d", i);
            strcpy(shared->name_of_semaphore_ans,sem_name_ans);
            shared_t->line_start = shared->line_start;
            shared_t->line_end = shared->line_end;
            if (sem_post(semaphore_wake) < 0) {
                perror("sem_post(3) error on child");
            }
            printf("name is %s\n",name);
            int j = 0;
            while (1) {
                if(j>=shared_t->line_start){
                    printf("my bad %s\n",sem_name);
                    // int value = 0;
                    printf("I IS %d\n",i);
                    // int ret_val = sem_getvalue(semaphore_transactions[i],value);
                    // printf("Semaphore before wait has value:%d and ret_val:%d\n in child",value,ret_val);
                    if (sem_wait(semaphore_transactions[i]) < 0) {
                    printf("something went wrong\n");
                    continue;
                }
                printf("GOT LINE____ %s", shared_t->line);
                if (sem_post(semaphore_transactions_ans[i]) < 0) {
                    perror("sem_post(3) failed");
                    continue;
                }
            }
            if(j >= shared_t->line_end){
                break;
            }
            j++;
        }
        if (sem_wait(semaphore_answer) < 0) {
                perror("sem_wait(3) failed on child");
                continue;
            }
            printf("got the answer!\n");
            if (sem_post(semaphore_req) < 0) {
                perror("sem_post(3) error on child");
            }
            sleep(2);
        }
        if (shmdt(shared_temp) == -1) {
		    fprintf(stderr, "shmdt failed\n");
		    exit(EXIT_FAILURE);
	    }
        printf("child ending...\n");
        exit(0);
    }
    else{
        printf("Parent!\n");
        pthread_t thread_id[numberOfProcesses*numOfFilesWanted];
        for(int i =0;i < numberOfProcesses*numOfFilesWanted;i++){
            if (sem_wait(semaphore_wake) < 0) {
                perror("sem_wait(3) failed on child");
                continue;
            }
            printf("TAKE %s\n",shared->name_of_file);
            struct args *args = malloc (sizeof (struct args));
            printf("malloc failed!\n");
            strcpy(args->name_of_file,shared->name_of_file);
            strcpy(args->name_of_semaphore,shared->name_of_semaphore);
            strcpy(args->name_of_semaphore_ans,shared->name_of_semaphore_ans);
            args->temp_id = shared->temp_id;
            args->line_start = shared->line_start;
            args->line_end = shared->line_end;
            printf("HERE???%d\n",i);   
            pthread_create (&thread_id[i], NULL, &threadWorker, (void*) args);
            // if (sem_post(semaphore_answer) < 0) {
            //     perror("sem_post(3) error on child");
            // }
            printf("posted sem\n");
        }
        void* status;
        for (int threadCount = 0; threadCount< numberOfProcesses*numOfFilesWanted; threadCount++) { 
		    pthread_join(thread_id[threadCount], &status);
		    printf("Parent: completed join with thread %ld having a status of %ld\n",threadCount,(long)status);
        }
    }
    
    shared = (struct shared_struct *)shared_memory;
    if (shmdt(shared_memory) == -1) {
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
    for (int i = 0; i < numberOfProcesses; i++)
        if (waitpid(pid[i], NULL, 0) < 0)
            perror("waitpid(2) failed");
    if (sem_close(semaphore_req) < 0) {
        perror("sem_close(3) failed");
        /* We ignore possible sem_unlink(3) errors here */
        sem_unlink(SEM_NAME1);
        exit(EXIT_FAILURE);
    }
    if (sem_close(semaphore_wake) < 0) {
        perror("sem_close(3) failed");
        /* We ignore possible sem_unlink(3) errors here */
        sem_unlink(SEM_NAME2);
        exit(EXIT_FAILURE);
    }
    if (sem_close(semaphore_answer) < 0) {
        perror("sem_close(3) failed");
        /* We ignore possible sem_unlink(3) errors here */
        sem_unlink(SEM_NAME3);
        exit(EXIT_FAILURE);
    }
    if (sem_unlink(SEM_NAME1) < 0)
        perror("sem_unlink(3) failed");
    if (sem_unlink(SEM_NAME2) < 0)
        perror("sem_unlink(3) failed");
    if (sem_unlink(SEM_NAME3) < 0)
        perror("sem_unlink(3) failed");
    printf("what?\n");    
    for(i = 0; i < numberOfProcesses; i++){
        char sem_name[1024] = "TS";
        sprintf(sem_name, "TS%d", i);
        if (sem_close(semaphore_transactions[i]) < 0) {               
            perror("sem_close(3) failed");
            sem_unlink(sem_name);
            exit(EXIT_FAILURE);
        }
        if (sem_unlink(sem_name) < 0)
            perror("sem_unlink(3) failed");
    }
    for(i = 0; i < numberOfProcesses; i++){
        char sem_name[1024] = "TS_ANS";
        sprintf(sem_name, "TS_ANS%d", i);
        if (sem_close(semaphore_transactions_ans[i]) < 0) {               
            perror("sem_close(3) failed");
            sem_unlink(sem_name);
            exit(EXIT_FAILURE);
        }
        if (sem_unlink(sem_name) < 0)
            perror("sem_unlink(3) failed");
    }
    printf("ALL DONE!\n");
    return 0;
}