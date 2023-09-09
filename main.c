#include "headers.h"
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
    struct shared_struct * shared =  (struct shared_struct *) shared_memory;
    shared->file_num = 1;
    int numberOfProcesses = atoi(argv[1]);
    sem_t *semaphore_transactions[numberOfProcesses];
    sem_t *semaphore_transactions_ans[numberOfProcesses];
    pid_t pid[numberOfProcesses];
    int numOfFilesWanted = atoi(argv[2]);
    int lambda = atoi(argv[3]);
    int i = 0;
    for(i = 0; i < numberOfProcesses; i++){
        char sem_name[1024];
        sprintf(sem_name, "TS%d", i);
        semaphore_transactions[i] = sem_open(sem_name, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE_WAKE);
        if (semaphore_transactions[i] == SEM_FAILED) {
            perror("sem_open(3) error");
            exit(EXIT_FAILURE);
        }
    }
    for(i = 0; i < numberOfProcesses; i++){
        char sem_name[1024];
        sprintf(sem_name, "TS_ANS%d", i);
        semaphore_transactions_ans[i] = sem_open(sem_name, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE_WAKE);
        if (semaphore_transactions_ans[i] == SEM_FAILED) {
            perror("sem_open(3) error");
            exit(EXIT_FAILURE);
        }
    }
    for(i = 0; i < numberOfProcesses; i++){
        pid[i] = fork();
        if(!pid[i]){
            work(numOfFilesWanted,semaphore_req,i,shared,semaphore_wake,semaphore_transactions[i],semaphore_transactions_ans[i],semaphore_answer,lambda);
        }
    }
    printf("parent!\n");
    pthread_t thread_id[numberOfProcesses*numOfFilesWanted];
    for(int i =0;i < numberOfProcesses*numOfFilesWanted;i++){
        if (sem_wait(semaphore_wake) < 0) {
            perror("sem_wait(3) failed on child");
            continue;
        }
        printf("waked up\n");
        struct args *args = malloc (sizeof (struct args));
        strcpy(args->name_of_file,shared->name_of_file);
        strcpy(args->name_of_semaphore,shared->name_of_semaphore);
        strcpy(args->name_of_semaphore_ans,shared->name_of_semaphore_ans);
        args->temp_id = shared->temp_id;
        args->line_start = shared->line_start;
        args->line_end = shared->line_end;  
        pthread_create (&thread_id[i], NULL, &threadWorker, (void*) args);
    }
        void* status;
        for (int threadCount = 0; threadCount< numberOfProcesses*numOfFilesWanted; threadCount++) { 
		    pthread_join(thread_id[threadCount], &status);
		    printf("Parent: completed join with thread %d having a status of %ld\n",threadCount,(long)status);
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
        sem_unlink(SEM_NAME1);
        exit(EXIT_FAILURE);
    }
    if (sem_close(semaphore_wake) < 0) {
        perror("sem_close(3) failed");
        sem_unlink(SEM_NAME2);
        exit(EXIT_FAILURE);
    }
    if (sem_close(semaphore_answer) < 0) {
        perror("sem_close(3) failed");
        sem_unlink(SEM_NAME3);
        exit(EXIT_FAILURE);
    }
    if (sem_unlink(SEM_NAME1) < 0)
        perror("sem_unlink(3) failed");
    if (sem_unlink(SEM_NAME2) < 0)
        perror("sem_unlink(3) failed");
    if (sem_unlink(SEM_NAME3) < 0)
        perror("sem_unlink(3) failed");   
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