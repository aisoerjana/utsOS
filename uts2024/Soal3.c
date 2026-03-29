#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

//Deklarasi Variabel Global
#define CONTESTANT 3
int cangkirTersedia;
int cangkirTerkonsumsi[CONTESTANT] = {0};
pthread_mutex_t lock;

//Fungsi minumKopi (Thread Kontestan)
void *minumKopi(void *arg){
    int id = *(int *)arg; //Mengambil id kontestan
    while (1){
        pthread_mutex_lock(&lock); //Mengunci mutex sebelum mengakses variabel global
        //Jika cangkirTersedia <= 0
        if(cangkirTersedia <= 0){
            pthread_mutex_unlock(&lock); //Membuka mutex sebelum keluar dari loop
            break;
        }
        cangkirTersedia--; //Mengurangi jumlah kopi tersedia
        cangkirTerkonsumsi[id]++; //Menambah jumlah kopi yang dikonsumsi kontestan
        printf("Kontestan %d minum 1 cangkir kopi. Sisa cangkir kopi: %d\n", id+1, cangkirTersedia);
        pthread_mutex_unlock(&lock); //Membuka mutex setelah mengakses variabel gl
        sleep(rand() % 5); //Simulasi waktu minum kopi berupa waktu random (0 - 4 detik).
    } 
    return NULL;
}

//Fungsi tampilkanStatus
void *tampilkanStatus(void *arg){
    while(1){
        pthread_mutex_lock(&lock); //Mengunci mutex sebelum membaca var global
        //Jika cangkirTersedia <= 0
        if(cangkirTersedia <= 0){
            pthread_mutex_unlock(&lock); //Membuka mutex sebelum keluar dari loop
            break;
        }
        printf("\tStatus: Sisa cangkir %d\n\tKontestan 1 : %d Cangkir, Kontestan 2 : %d Cangkir, Kontestan 3 : %d Cangkir\n", 
            cangkirTersedia, cangkirTerkonsumsi[0], cangkirTerkonsumsi[1], cangkirTerkonsumsi[2]);
        pthread_mutex_unlock(&lock);//Membuka mutex setelah membaca variabel global 
        sleep(2); //Update Status tiap 2 detik (delay)
    }
    return NULL;
}

// Fungsi Utama
int main (int argc, char *argv[]) {
    srand(time(NULL)); //Inisialisasi waktu random dengan seed pada fungsi srand(time(NULL))
    printf("Masukkan jumlah cangkir kopi (11 - 199): ");
    scanf("%d", &cangkirTersedia);

    // Validasi Input jumlah cangkir kopi tersedia yang diinput
    if(cangkirTersedia < 11 || cangkirTersedia > 199){
        printf("Jumlah cangkir harus berjumlah antara 11 - 199!\n");
        return 1;
    }

    //Inisialisasi array untuk menyimpan 3 thread kontestan, serta thread khusus untuk menampilkan status kompetisi
    pthread_t kontenstan[CONTESTANT], status_thread;
    pthread_mutex_init(&lock, NULL); //Inisialisasi mutex lock untuk menghindari race condition saat mengakses var cangkirTersedia

    //Buat Thread untuk tiap kontestan, dan diberi id unik sesuai index array
    int ids[CONTESTANT] = {0, 1, 2};
    for(int i = 0; i < CONTESTANT; i++){
        //Tiap thread akan menjalankan fungsi minumKopi
        pthread_create(&kontenstan[i], NULL, minumKopi, &ids[i]);
    }
    //Buat thread untuk menjalankan fungsi tampilkanStatus
    pthread_create(&status_thread, NULL, tampilkanStatus, NULL);

    //Perulangan untuk menunggu semua thread kontestan selesai
    for(int i = 0; i < CONTESTANT; i++){
        pthread_join(kontenstan[i], NULL);
    }
    //Menunggu thread status selesai
    pthread_join(status_thread, NULL);

    //Mutex lock akan dihancurkan untuk membebaskan resource
    pthread_mutex_destroy(&lock);

    // Untuk menentukan juara berdasarkan cangkirTerkonsumsi
    printf("\n=== Hasil Akhir ===\n");
    int ranking[CONTESTANT] = {0, 1, 2};
    for(int i = 0; i < CONTESTANT - 1; i++){
        for(int j = i + 1; j < CONTESTANT; j++){
            if(cangkirTerkonsumsi[ranking[i]] < cangkirTerkonsumsi[ranking[j]]){
                int temp = ranking[i];
                ranking[i] = ranking[j];
                ranking[j] = temp;
            }
        }
    }
    // Perulangan untuk melakukan print hasil kompetisi
    for(int i = 0; i < CONTESTANT; i++){
        printf("\nJuara %d: Kontestan %d dengan %d cangkir kopi\n", i + 1, ranking[i] + 1, cangkirTerkonsumsi[ranking[i]]);
    }
    return 0;
}
