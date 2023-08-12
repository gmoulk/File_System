#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <sys/shm.h>

struct temp_struct {
	int numOfLine;
	char file[10][2048];
};

struct shared_struct {
	int file_num;
	char text[2048];
};


int main(int argc,char** argv){
    void *shared_memory = (void *)0;
	struct shared_stuct *shared;
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
	printf("Shared memory segment with id %d attached at %p\n", shmid, shared_memory);
    int numberOfProcesses = atoi(argv[1]);
    printf("Argument is %d\n",numberOfProcesses);
    int pid = 0;
    for(int i = 0; i < numberOfProcesses; i++){
        pid = fork();
        if(!pid){
            break;
        }
    }
    if (pid == 0){
        printf("Inside child\n");
        printf("Process %8d\n",getpid());
        char str[100];
        sprintf(str, "%8d", getpid());
        FILE *fp;
        fp = fopen(str, "w"); 
        key_t key;
        if ((key = ftok(str, 'R')) == -1) {
		    perror("ftok");
		    exit(1);
	    }
        void *shared_temp = (void *)0;
	    struct temp_struct *shared;
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
	    printf("Shared temp segment with id %d attached at %p\n", shmid, shared_memory);
        shared = (struct shared_temp *)shared_temp;
        if (shmdt(shared_memory) == -1) {
		    fprintf(stderr, "shmdt failed\n");
		    exit(EXIT_FAILURE);
	    }
    }
    
    struct dirent *de;  // Pointer for directory entry
    shared = (struct shared_struct *)shared_memory;
    if (shmdt(shared_memory) == -1) {
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
    
  
    // // opendir() returns a pointer of DIR type. 
    // DIR *dr = opendir(".");
  
    // if (dr == NULL)  // opendir returns NULL if couldn't open directory
    // {
    //     printf("Could not open current directory" );
    //     return 0;
    // }
  
    // // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // // for readdir()
    // srand(time(NULL));
    // int randomNum = (rand() % 6) + 1;
    // int it = 0;
    // char name[2048];
    // printf("Random number is %d\n",randomNum);
    // while ((de = readdir(dr)) != NULL ){
    //     if(de->d_type == 8){
    //         printf("%s %d\n", de->d_name ,de->d_type);
    //         it++;
    //         if(it == randomNum){
    //             strcpy(name,de->d_name);
    //         }
    //     }

    // }
    // printf("name is %s\n",name);
  
    // closedir(dr);    
    // return 0;
}