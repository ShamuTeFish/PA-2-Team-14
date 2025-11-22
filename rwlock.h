#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct{
    pthread_mutex_t mutex;
    pthread_cond_t can_read;
    pthread_cond_t can_write;

    int readers;
    int writer_active;
    int writers_waiting;
} rwlock_t;

void rwlock_init(rwlock_t * l);
void rwlock_read_lock(rwlock_t *l);
void rwlock_read_unlock(rwlock_t *l);
void rwlock_write_lock(rwlock_t *l);
void rwlock_write_unlock(rwlock_t *l);