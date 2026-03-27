#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>

struct Data {
    int total_ordered;
    int current_records;
    int total_cooked;
};

void sem_lock(int semid) {
    struct sembuf sb = {0, -1, 0};
    semop(semid, &sb, 1);
}

void sem_unlock(int semid) {
    struct sembuf sb = {0, 1, 0};
    semop(semid, &sb, 1);
}

int main() {
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(struct Data), 0666 | IPC_CREAT);
    struct Data *data = (struct Data *)shmat(shmid, NULL, 0);
    int semid = semget(key, 1, 0666 | IPC_CREAT);

    semctl(semid, 0, SETVAL, 1);
    srand(time(NULL));

    while (1) {
        sleep(rand() % 3 + 1); 
        sem_lock(semid);
        if (data->current_records < 30) { 
            data->total_ordered++;
            data->current_records++;
            printf("ORDER: Pesanan Masuk! Total: %d, Di Meja: %d, Sudah Dimasak: %d\n", 
                    data->total_ordered, data->current_records, data->total_cooked);
        } else {
            printf("ORDER: Penuh woi (30), tunggu koki...\n");
        }
        sem_unlock(semid);
    }
    return 0;
}




