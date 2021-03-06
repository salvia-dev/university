// #############################
// Dawid Bytys Krakow 06.04.2022
// #############################
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define CHILDREN_NUM 3

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    printf("Invalid number of arguments.\n");
    exit(EXIT_FAILURE);
  }

  int signal_num = atoi(argv[2]);
  if (signal_num == 9 || signal_num == 19) {
    printf("Cannot handle this type of signal, try different one\n");
    exit(EXIT_FAILURE);
  }

  pid_t process = fork();

  if (process == -1) {
    perror("fork error");
    exit(EXIT_FAILURE);
  } else if (process == 0) {
    if (setpgid(process, 0) == -1) {
      perror("failed to set group lider");
      exit(EXIT_FAILURE);
    }

    if (signal(signal_num, SIG_IGN) == SIG_ERR) {
      perror("Signal error");
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < CHILDREN_NUM; i++) {
      pid_t child_process = fork();

      if (child_process == -1) {
        perror("child fork error");
        exit(EXIT_FAILURE);
      } else if (child_process == 0) {
        if (execl("./signal.x", "signal.x", argv[1], argv[2], NULL) == -1) {
          perror("failed to exec file");
          exit(EXIT_FAILURE);
        }
      } else {
        sleep(1);
      }
    }

    for (int i = 0; i < CHILDREN_NUM; i++) {
      pid_t process_end;
      int status;

      if ((process_end = wait(&status)) == -1) {
        perror("wait error");
        exit(EXIT_FAILURE);
      }

      printf("Process %d has been terminated with status %d\n", process_end,
             status);
    }
  } else {
    sleep(3);

    if (kill(-getpgid(process), 0) == -1) {
      perror("Process does not exist.");
      exit(EXIT_FAILURE);
    }

    if (kill(-getpgid(process), signal_num) == -1) {
      perror("kill error");
      exit(EXIT_FAILURE);
    }

    if (wait(NULL) == -1) {
      perror("wait error");
      exit(EXIT_FAILURE);
    }
  }

  return 0;
}
