#include <semaphore.h>
#include <stdlib.h>

int main() {
  // Create a semaphore and initialize it to 1.
  sem_t sem;
  sem_init(&sem, 0, 1);

  // Create three child processes.
  pid_t pid1, pid2, pid3;
  pid1 = fork();
  if (pid1 == 0) {
    // Child process 1.
    sem_wait(&sem);
    // Do something with the shared resource.
    sem_post(&sem);
    return 0;
  }

  pid2 = fork();
  if (pid2 == 0) {
    // Child process 2.
    sem_wait(&sem);
    // Do something with the shared resource.
    sem_post(&sem);
    return 0;
  }

  pid3 = fork();
  if (pid3 == 0) {
    // Child process 3.
    sem_wait(&sem);
    // Do something with the shared resource.
    sem_post(&sem);
    return 0;
  }

  // Parent process.
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);
  waitpid(pid3, NULL, 0);
  return 0;
}
