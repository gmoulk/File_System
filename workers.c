#include "headers.h"
int expodential_distributed_number_generator(int lambda_parameter,int i){
    srand(time(NULL) + i);
    double formal = rand() / (RAND_MAX + 1.0);
    int return_num = (int) ((-log(1- formal) / lambda_parameter)*10);
    return return_num;
}

void work(int numOfFilesWanted,sem_t* semaphore_req,int i,struct shared_struct * shared,sem_t* semaphore_wake,sem_t* semaphore_transactions,sem_t* semaphore_transactions_ans,sem_t* semaphore_answer,int lambda){
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
    shared_t = (struct temp_struct *)shared_temp;
    double sum = 0;
    for(int k = 0;k < numOfFilesWanted;k++){
        clock_t start, end;
        start = clock();
        struct dirent *de;
        // opendir() returns a pointer of DIR type. 
        DIR *dr = opendir("./test_folder");
        if (dr == NULL)  // opendir returns NULL if couldn't open directory
        {
            printf("Could not open current directory" );
        }
        srand(time(NULL) + i);
        int randomNum = (rand() % 3) + 1;
        int it = 0;
        char name[2048];
        while ((de = readdir(dr)) != NULL ){
            if(de->d_type == 8){
                it++;
                if(it == randomNum){
                    strcpy(name,de->d_name);
                }
            }
        }
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
        printf("waiting semaphore request!\n");
        shared_t = (struct temp_struct *)shared_temp;
        if (sem_wait(semaphore_req) < 0) {
            perror("sem_wait(3) failed on child");
            continue;
        }
        strcpy(shared->name_of_file,name);
        shared->line_end = rand() % MAX_LINES;
        shared->line_start = rand() % (shared->line_end);
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
        end = clock();
        double time_taken = ((double) (end - start))/CLOCKS_PER_SEC;
        fprintf(fp,"Request %d was file with name %s and took %f secs from creating to acceptance\n",k,name,time_taken);
        int j = 0;
        printf("waiting semaphore answer!\n");
        if (sem_wait(semaphore_answer) < 0) {
            perror("sem_wait(3) failed on child");
            continue;
        }
        printf("posting request!\n");
        if (sem_post(semaphore_req) < 0) {
            perror("sem_post(3) error on child");
        }
        printf("posted request!\n");
        while (1) {
            if(j>=shared_t->line_start){
                if (sem_wait(semaphore_transactions) < 0) {
                    printf("something went wrong\n");
                    continue;
                }
                // printf("GOT LINE____ %s", shared_t->line);
                if (sem_post(semaphore_transactions_ans) < 0) {
                    perror("sem_post(3) failed");
                    continue;
                }
            }
            if(j >= shared_t->line_end){
                break;
            }
            j++;
        }
        if (shmdt(shared_temp) == -1) {
		    fprintf(stderr, "shmdt failed\n");
		    exit(EXIT_FAILURE);
	    }
        int random_number = expodential_distributed_number_generator(lambda,i);
        sum += random_number;
        printf("done one loop\n");    
            // sleep(random_number);
        }
        sum = sum/numOfFilesWanted;
        fprintf(fp, "Average time between reqs is %f\n",sum);
        fclose(fp);
        printf("exited\n");
        exit(0);
}

void* threadWorker(void* args){
    sem_t *semaphore = sem_open(SEM_NAME3, O_RDWR);
    if (semaphore == SEM_FAILED) {
        printf("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    void *shared_temp = (void *)0;
	struct temp_struct *shared_t;
    struct args* args_ = (struct args*)args;
    int shmid = args_->temp_id;
    shared_temp = shmat(shmid, (void *)0, 0);
	if (shared_temp == (void *)-1) {
	 	fprintf(stderr, "shmat failed\n");
	 	exit(EXIT_FAILURE);
	}
    shared_t = (struct temp_struct *)shared_temp;
    char name_of_semaphore[2048];
    char name_of_semaphore_ans[2048];
    char name_of_temp[2048];
    strcpy(name_of_semaphore,args_->name_of_semaphore);
    strcpy(name_of_semaphore_ans,args_->name_of_semaphore_ans);
    strcpy(name_of_temp,args_->name_of_semaphore);
    sem_t * transactions_sem = sem_open(name_of_semaphore, O_RDWR);
    if (transactions_sem == SEM_FAILED) {
        printf("failed seamaphore inititalization\n");
    }
    sem_t * transactions_sem_ans = sem_open(name_of_semaphore_ans, O_RDWR);
    if (transactions_sem_ans == SEM_FAILED) {
        printf("failed seamaphore inititalization\n");
    }
    int line_start = args_->line_start;
    int line_end = args_->line_end;
    char filename[3096];
    sprintf(filename,"./test_folder/%s",args_->name_of_file);
    fp = fopen(filename, "r");
    if (fp == NULL)
        printf("didnt find file\n");
    int i = 0;
    printf("gona post semaphor ans!\n");
    if (sem_post(semaphore) < 0) {
        printf("sem_post(3) error on child");
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        if(i>=line_start){
            strcpy(shared_t->line,line);
            if (sem_post(transactions_sem) < 0) {
                printf("sem_post(3) failed");
                continue;
            }
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
    pthread_exit((void*)EXIT_SUCCESS);
}