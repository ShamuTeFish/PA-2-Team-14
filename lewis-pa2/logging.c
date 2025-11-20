// logging.c
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/time.h>
#include "logging.h"

static FILE *log_fp = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

long long current_timestamp(void) {
    struct timeval te;
    gettimeofday(&te, NULL);
    long long microseconds = (long long)te.tv_sec * 1000000LL + te.tv_usec;
    return microseconds;
}

void log_init(const char *filename) {
    log_fp = fopen(filename, "w");
    if (!log_fp) {
        perror("hash.log");
        exit(1);
    }
}

void log_close(void) {
    if (log_fp) {
        fclose(log_fp);
        log_fp = NULL;
    }
}

void log_simple(int priority, const char *text) {
    if (!log_fp) return;
    long long ts = current_timestamp();

    pthread_mutex_lock(&log_mutex);

    fprintf(log_fp, "%lld: THREAD %d%s\n", ts, priority, text);
    fflush(log_fp);
    pthread_mutex_unlock(&log_mutex);
}

void log_command(int priority, const char *fmt, ...) {
    if (!log_fp) return;
    long long ts = current_timestamp();

    pthread_mutex_lock(&log_mutex);
    fprintf(log_fp, "%lld: THREAD %d", ts, priority);

    va_list ap;
    va_start(ap, fmt);
    vfprintf(log_fp, fmt, ap);  // caller includes leading comma
    va_end(ap);

    fprintf(log_fp, "\n");
    fflush(log_fp);
    pthread_mutex_unlock(&log_mutex);
}
