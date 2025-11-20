// chash.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>

#include "hash.h"
#include "rwlock.h"
#include "logging.h"

typedef enum {
    CMD_INSERT,
    CMD_DELETE,
    CMD_UPDATE,
    CMD_SEARCH,
    CMD_PRINT
} CommandType;

typedef struct {
    CommandType type;
    char name[50];
    uint32_t salary;   // 0 if unused
    int priority;
} Command;

#define MAX_COMMANDS 1024

static rwlock_t list_lock;

// Priority scheduler
static pthread_mutex_t sched_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  sched_cv    = PTHREAD_COND_INITIALIZER;
static int next_priority = 0;

static Command commands[MAX_COMMANDS];
static int num_commands = 0;

void *command_thread(void *arg) {
    Command *cmd = (Command *)arg;

    // Waiting
    log_simple(cmd->priority, "WAITING FOR MY TURN");

    pthread_mutex_lock(&sched_mutex);
    while (cmd->priority != next_priority) {
        pthread_cond_wait(&sched_cv, &sched_mutex);
    }
    // Awakened
    log_simple(cmd->priority, "AWAKENED FOR WORK");
    pthread_mutex_unlock(&sched_mutex);

    // Execute command
    if (cmd->type == CMD_INSERT) {
        uint32_t h = jenkins_hash(cmd->name);
        log_command(cmd->priority, ",INSERT,%u,%s,%u", h, cmd->name, cmd->salary);

        rwlock_acquire_write(&list_lock, cmd->priority);
        hash_insert(h, cmd->name, cmd->salary);
        rwlock_release_write(&list_lock, cmd->priority);

    } else if (cmd->type == CMD_DELETE) {
        uint32_t h = jenkins_hash(cmd->name);
        log_command(cmd->priority, ",DELETE,%u,%s", h, cmd->name);

        rwlock_acquire_write(&list_lock, cmd->priority);
        hash_delete(h, cmd->name);
        rwlock_release_write(&list_lock, cmd->priority);

    } else if (cmd->type == CMD_UPDATE) {
        uint32_t h = jenkins_hash(cmd->name);
        log_command(cmd->priority, ",UPDATE,%u,%s,%u", h, cmd->name, cmd->salary);

        rwlock_acquire_write(&list_lock, cmd->priority);
        hash_update(h, cmd->name, cmd->salary);
        rwlock_release_write(&list_lock, cmd->priority);

    } else if (cmd->type == CMD_SEARCH) {
        uint32_t h = jenkins_hash(cmd->name);
        log_command(cmd->priority, ",SEARCH,%u,%s", h, cmd->name);

        rwlock_acquire_read(&list_lock, cmd->priority);
        hash_search(h, cmd->name);
        rwlock_release_read(&list_lock, cmd->priority);

    } else if (cmd->type == CMD_PRINT) {
        log_command(cmd->priority, ",PRINT");

        rwlock_acquire_read(&list_lock, cmd->priority);
        hash_print_all();
        rwlock_release_read(&list_lock, cmd->priority);
    }

    // Tell next priority thread to wake up
    pthread_mutex_lock(&sched_mutex);
    next_priority++;
    pthread_cond_broadcast(&sched_cv);
    pthread_mutex_unlock(&sched_mutex);

    return NULL;
}

static CommandType parse_command_type(const char *cmd) {
    if (strcmp(cmd, "insert") == 0) return CMD_INSERT;
    if (strcmp(cmd, "delete") == 0) return CMD_DELETE;
    if (strcmp(cmd, "update") == 0) return CMD_UPDATE;
    if (strcmp(cmd, "search") == 0) return CMD_SEARCH;
    if (strcmp(cmd, "print") == 0)  return CMD_PRINT;

    fprintf(stderr, "Unknown command: %s\n", cmd);
    exit(1);
}

static void read_commands_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("commands.txt");
        exit(1);
    }

    char line[256];

    // First line: threads,<num>,0
    if (!fgets(line, sizeof(line), fp)) {
        fprintf(stderr, "Empty commands file\n");
        exit(1);
    }

    char *tok = strtok(line, ",\n");
    if (!tok || strcmp(tok, "threads") != 0) {
        fprintf(stderr, "First line must start with 'threads'\n");
        exit(1);
    }

    tok = strtok(NULL, ",\n");
    if (!tok) {
        fprintf(stderr, "Missing thread count\n");
        exit(1);
    }
    int expected = atoi(tok);

    int count = 0;
    while (fgets(line, sizeof(line), fp) && count < MAX_COMMANDS) {
        // skip blank lines
        if (line[0] == '\n' || line[0] == '\r') continue;

        char *cmd_str = strtok(line, ",\n");
        char *name    = strtok(NULL, ",\n");
        char *sal_str = strtok(NULL, ",\n");
        char *pri_str = strtok(NULL, ",\n");

        if (!cmd_str || !name || !sal_str || !pri_str) {
            continue; // malformed
        }

        Command *c = &commands[count];
        c->type = parse_command_type(cmd_str);
        c->priority = atoi(pri_str);
        c->salary = (uint32_t)strtoul(sal_str, NULL, 10);

        if (c->type == CMD_PRINT) {
            c->name[0] = '\0';
        } else {
            strncpy(c->name, name, sizeof(c->name) - 1);
            c->name[sizeof(c->name) - 1] = '\0';
        }

        count++;
    }

    fclose(fp);

    num_commands = count;
    if (expected != count) {
        // Not fatal, but nice to warn
        fprintf(stderr, "Warning: expected %d commands, read %d\n", expected, count);
    }
}

int main(void) {
    // Hard-coded file name as required
    read_commands_file("commands.txt");

    hash_init();
    rwlock_init(&list_lock);
    log_init("hash.log");

    pthread_t threads[MAX_COMMANDS];

    next_priority = 0;

    for (int i = 0; i < num_commands; i++) {
        if (pthread_create(&threads[i], NULL, command_thread, &commands[i]) != 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    for (int i = 0; i < num_commands; i++) {
        pthread_join(threads[i], NULL);
    }

    log_close();
    hash_destroy();

    return 0;
}
