#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

// Inisialisasi Size Buffer
#define MAX_LEN 100
// Fungsi untuk mengubah string menjadi UpperCase
void toUpperCase(char *str){
    int i;
    for(i = 0; str[i] != '\0'; i++ ){
        if (str[i] >= 'a' && str[i] <= 'z'){
            str[i] = str[i] - 32;
        }
    }
}

//Fungsi Utama
int main (int argc, char *argv[]) {
    int status; // Inisialisasi status
    int fdescriptor[2]; // File Descriptor untuk Pipe
    char input[MAX_LEN];
    pid_t pidB, pidC, pidD;

    // Validasi pada fdescriptor
    if (pipe(fdescriptor) == -1){
        perror("PIPE GAGAL DIBUAT");
        exit(1);
    }
        
    // Process A menerima Input dari USER
    printf("Masukkan string: ");
    fgets(input, MAX_LEN, stdin);
    input[strcspn(input, "\n")] = 0;
    printf("Process A: PID = %d, Parent_PID = %d : %s\n", getpid(), getppid(), input);

    // Membuat Process B
    pidB = fork();
    if(pidB == 0){
        printf("Process B: PID = %d, Parent_PID = %d : %s\n", getpid(), getppid(), input);

        // Membuat Process D sebagai anak dari Process B
        pidD = fork();
        if (pidD == 0){
            printf("Process D: PID = %d, Parent_PID = %d : %s\n", getpid(), getppid(), input);
            exit(0);
        }

        // Process B menunggu Process D selesai
        waitpid(pidD, &status, 0);
        exit(0);
    }
    // Process A menunggu Process B selesai
    waitpid(pidB, &status, 0);

    // Membuat Process C
    pidC = fork();
    if (pidC == 0){
        close(fdescriptor[1]); // Menutup penulisan pipe di Process C
        char buffer[MAX_LEN];

        read(fdescriptor[0], buffer,MAX_LEN);
        toUpperCase(buffer);
        printf("Process C: PID = %d, Parent_PID = %d : %s\n", getpid(), getppid(), buffer);
        close(fdescriptor[0]);

        exit(0);
    }

    // Process A menulis ke pipe setelah forking C
    close(fdescriptor[0]); // Menutup read pipe di process A
    write(fdescriptor[1], input, strlen(input) + 1);
    close(fdescriptor[1]); // Menutup write pipe setelah menulis

    // Menunggu proses C selesai
    waitpid(pidC, &status, 0); // Tunggu proses C selesai

    return 0;
}
