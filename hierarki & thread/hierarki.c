#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid2, pid3, pid4, pid5, pid6;
    
     printf("Process 1: %d : %d\n", getpid(), getppid());

    pid2 = fork();
    if (pid2 == 0) {
        printf("Process 2: %d : %d\n", getpid(), getppid());
        exit(0);  
    }

    pid3 = fork();
    if (pid3 == 0) {
        printf("Process 3: %d : %d\n", getpid(), getppid());

        pid5 = fork();
        if (pid5 == 0) {
            printf("Process 5: %d : %d\n", getpid(), getppid());
            exit(0);
        }

        waitpid(pid5, NULL, 0);
        exit(0);
    }

    pid4 = fork();
    if (pid4 == 0) {
        printf("Process 4: %d : %d\n", getpid(), getppid());

        pid6 = fork();
        if (pid6 == 0) {
            printf("Process 6: %d : %d\n", getpid(), getppid());
            exit(0);
        }

        waitpid(pid6, NULL, 0); 
        exit(0);
    }
    
       	waitpid(pid2, NULL, 0); 
       	waitpid(pid3, NULL, 0); 
       	waitpid(pid4, NULL, 0); 
       
    printf("Semua child selesai.\n");
    return 0;
    
}

