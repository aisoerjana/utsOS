#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//CATATAN :
//Program order.c harus dijalankan dulu sebelum program cook.c

#define SEM_MUTEX 2

// Struct untuk menyimpan var total_orders dan cooked_orders
typedef struct {
    int total_orders;
    int cooked_orders;
} SharedData;

//Deklarasi Variabel Global
int shm_id, sem_id;
SharedData *shared_data;

// Fungsi Operasi Semaphore
void semaphoreOperation(int sem_id, int sem_num, int operation){
    struct sembuf sb = {sem_num, operation, 0};
    if(semop(sem_id, &sb, 1) == -1){
        perror("Gagal Mengubah Semaphore");
        exit(1);
    }
}

// Fungsi untuk cleanResource saat cook ditutup
void cleanResource(int signum){
    printf("\n[COOK] Membersihkan Resource.....\n");

    //Hapus link ke shared memory
    shmdt(shared_data);

    printf("[COOK] Chef Berhenti Masak.....\n");
    exit(0);
}

// Fungsi Utama
int main (int argc, char *argv[]) {
    srand(time(NULL)); //Simulasi waktu random

    // Buat SHM_KEY dan SEM_KEY dengan ftok
    key_t shm_key = ftok("order.c", 'A');
    key_t sem_key = ftok("order.c", 'B');

    //Signal Handler ketika CTRL+C
    signal(SIGINT, cleanResource);
    
    

    // Mengakses shared memory
    shm_id = shmget(shm_key, sizeof(SharedData), 0666);
    if (shm_id == -1){
        perror("shmget gagal");
        exit(1);
    }
    shared_data = (SharedData *)shmat(shm_id, NULL, 0);
    if(shared_data == (void *)-1){
        perror("shmat gagal");
        exit(1);
    }

    // Mengakses Semaphore
    sem_id = semget(sem_key, 3, 0666);
    if(sem_id == -1){
        perror("semget gagal");
        exit(1);
    }

    while(1){
        semaphoreOperation(sem_id, 1, -1); //Tunggu Pesanan (sem_full)
        semaphoreOperation(sem_id, SEM_MUTEX, -1); // SEMAPHORE MUTEX (Kunci Akses Memory)
        shared_data->cooked_orders++;
        printf("Pesanan Matang! Pesanan Selesai: %d dari %d\n", shared_data->cooked_orders, shared_data->total_orders);
        semaphoreOperation(sem_id, SEM_MUTEX, 1); // SEMAPHORE MUTEX (Lepas Kunci Memory)
        semaphoreOperation(sem_id, 0, 1); // Tambah Slot Kosong (sem_empty)

        sleep(rand() % 3 + 1); // Lagi Masak Hihiheha
    }
    return 0;
}
