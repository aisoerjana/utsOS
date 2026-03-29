#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_ORDERS 30 //Inisialisasi max order
#define SEM_MUTEX 2 //Semaphore tambahan untuk mengamankan akses shm

//CATATAN :
//Dalam program ini, program order.c harus dijalankan terlebih dahulu pak

// struct untuk menyimpan variabel total order yang masuk dan sudah dimasak
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

// Funi untuk cleanResource
void cleanResource(int signum){
    printf("\n[ORDER] Membersihkan Resource.....\n");

    //Hapus shared memory
    shmdt(shared_data);
    shmctl(shm_id, IPC_RMID, NULL);

    // Hapus Semaphore
    semctl(sem_id, 0, IPC_RMID, NULL);
    printf("[ORDER] Order ditutup.....\n");
    exit(0);
}

//Fungsi Utama
int main (int argc, char *argv[]) {
    srand(time(NULL)); //Untuk memberikan waktu secara acak

    // Buat SHM_KEY dan SEM_KEY dengan ftok
    key_t shm_key = ftok("order.c", 'A');
    key_t sem_key = ftok("order.c", 'B');

    //Signal Handler ketika CTRL+C
    signal(SIGINT, cleanResource);

    // Buat SharedMemory
    shm_id = shmget(shm_key, sizeof(SharedData), IPC_CREAT | 0666);
    if(shm_id == -1){
        perror("shmget gagal");
        exit(1);
    }

    shared_data = (SharedData *)shmat(shm_id, NULL, 0);
    if(shared_data == (void *)-1){
        perror("shmat gagal");
        exit(1);
    }

    // Buat Semaphore
    sem_id = semget(sem_key, 3, IPC_CREAT | 0666);
    if (sem_id == -1){
        perror("semget gagal");
        exit(1);
    }
    semctl(sem_id, 0, SETVAL, MAX_ORDERS); // sem_empty
    semctl(sem_id, 1, SETVAL, 0); // sem_full
    semctl(sem_id, SEM_MUTEX, SETVAL, 1); // Semaphore untuk mutex

    // Inisialisasi data pesanan
    shared_data->total_orders = 0;
    shared_data->cooked_orders = 0;

    printf("[ORDER] Sistem Pemesanan Aktif.\n");

    // Loop ketika pemesanan
    while(1){
        sleep(rand()% 3 + 1); // Simulasi pemesanan

        semaphoreOperation(sem_id, 0, -1); // Tunggu jika penuh (sem_empty)
        semaphoreOperation(sem_id, SEM_MUTEX, -1); //Semaphore MUTEX (Kunci Akses Memory)
        shared_data->total_orders++;
        printf("Order Sudah Masuk! Total Order: %d\n", shared_data->total_orders);
        semaphoreOperation(sem_id, SEM_MUTEX, 1); //Semaphore MUTEX (Lepas Kunci Memory)
        semaphoreOperation(sem_id, 1, 1); // Tambah sem_full 
    }
    return 0;
}
