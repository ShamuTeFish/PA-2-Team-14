#include "rwlock.h"
#include "chash.h"

void rwlock_init(rwlock_t *l) {
    pthread_mutex_init(&l->mutex, NULL);
    pthread_cond_init(&l->can_read, NULL);
    pthread_cond_init(&l->can_write, NULL);
    l->readers = 0;
    l->writer_active = 0;
    l->writers_waiting = 0;
}

void rwlock_read_lock(rwlock_t *l) {
    pthread_mutex_lock(&l->mutex);

    while (l->writer_active || l->writers_waiting > 0) {
        pthread_cond_wait(&l->can_read, &l->mutex);
    }

    l->readers++;
    pthread_mutex_unlock(&l->mutex);
    fprintf(hlog,"READ LOCK ACQUIRED\n");
    lockAcq++;
}

void rwlock_read_unlock(rwlock_t *l) {
    pthread_mutex_lock(&l->mutex);
    l->readers--;
    if (l->readers == 0) {
        pthread_cond_signal(&l->can_write);
    }
    pthread_mutex_unlock(&l->mutex);
    fprintf(hlog,"READ LOCK RELEASED\n");
    lockRel++;
}

void rwlock_write_lock(rwlock_t *l) {
    pthread_mutex_lock(&l->mutex);
    l->writers_waiting++;

    while (l->readers > 0 || l->writer_active) {
        pthread_cond_wait(&l->can_write, &l->mutex);
    }

    l->writers_waiting--;
    l->writer_active = 1;

    pthread_mutex_unlock(&l->mutex);
    fprintf(hlog,"WRITE LOCK ACQUIRED\n");
    lockAcq++;
}

void rwlock_write_unlock(rwlock_t *l) {
    pthread_mutex_lock(&l->mutex);
    l->writer_active = 0;

    if (l->writers_waiting > 0)
        pthread_cond_signal(&l->can_write);
    else
        pthread_cond_broadcast(&l->can_read);

    pthread_mutex_unlock(&l->mutex);
    fprintf(hlog,"WRITE LOCK RELEASED\n");
    lockRel++;
}
