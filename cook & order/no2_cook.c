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

    srand(time(NULL) + 1);

    while (1) {
        sleep(rand() % 4 + 2); 

        sem_lock(semid);
        if (data->current_records > 0) {
            data->current_records--;
            data->total_cooked++;
            printf("COOK: Pesanan Selesai! Total Order: %d, Sisa Catatan: %d, Total Matang: %d\n", 
                    data->total_ordered, data->current_records, data->total_cooked);
        } else {
            printf("COOK: pesanan tidak ada, koki istirahat...\n");
        }
        sem_unlock(semid);
    }
    return 0;
}
