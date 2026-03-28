#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

char username[50];
char password[50];
int login_status = 0;

pthread_mutex_t lock;

void* verification(void* arg) {
    pthread_mutex_lock(&lock);

    strcpy(username, "admin");
    strcpy(password, "123");

    sleep(1);

    if (strcmp(username, "admin") == 0 && strcmp(password, "123") == 0) {
        login_status = 1;
    } else {
        login_status = 0;
    }

    pthread_mutex_unlock(&lock);
    return NULL;
}

void* logging(void* arg) {
    pthread_mutex_lock(&lock);

    printf("Logging: User = %s | Status = %d\n", username, login_status);

    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_mutex_init(&lock, NULL);

    pthread_create(&t1, NULL, verification, NULL);
    pthread_create(&t2, NULL, logging, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&lock);

    return 0;
}
