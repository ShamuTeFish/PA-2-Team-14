// rwlock.c
#include "rwlock.h"
#include "logging.h"

void rwlock_init(rwlock_t *lock) {
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->readers_ok, NULL);
    pthread_cond_init(&lock->writers_ok, NULL);
    lock->readers = 0;
    lock->writers_waiting = 0;
    lock->writer_active = 0;
}

void rwlock_acquire_read(rwlock_t *lock, int priority) {
    pthread_mutex_lock(&lock->mutex);
    while (lock->writer_active || lock->writers_waiting > 0) {
        pthread_cond_wait(&lock->readers_ok, &lock->mutex);
    }
    lock->readers++;
    pthread_mutex_unlock(&lock->mutex);

    log_simple(priority, " READ LOCK ACQUIRED");
}

void rwlock_release_read(rwlock_t *lock, int priority) {
    pthread_mutex_lock(&lock->mutex);
    lock->readers--;
    if (lock->readers == 0 && lock->writers_waiting > 0) {
        pthread_cond_signal(&lock->writers_ok);
    }
    pthread_mutex_unlock(&lock->mutex);

    log_simple(priority, " READ LOCK RELEASED");
}

void rwlock_acquire_write(rwlock_t *lock, int priority) {
    pthread_mutex_lock(&lock->mutex);
    lock->writers_waiting++;
    while (lock->writer_active || lock->readers > 0) {
        pthread_cond_wait(&lock->writers_ok, &lock->mutex);
    }
    lock->writers_waiting--;
    lock->writer_active = 1;
    pthread_mutex_unlock(&lock->mutex);

    log_simple(priority, " WRITE LOCK ACQUIRED");
}

void rwlock_release_write(rwlock_t *lock, int priority) {
    pthread_mutex_lock(&lock->mutex);
    lock->writer_active = 0;
    if (lock->writers_waiting > 0) {
        pthread_cond_signal(&lock->writers_ok);
    } else {
        pthread_cond_broadcast(&lock->readers_ok);
    }
    pthread_mutex_unlock(&lock->mutex);

    log_simple(priority, " WRITE LOCK RELEASED");
}
