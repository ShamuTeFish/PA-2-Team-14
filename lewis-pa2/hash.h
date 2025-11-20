// hash.h
#ifndef HASH_H
#define HASH_H

#include <stdint.h>

typedef struct hash_struct {
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;

// Initialize / destroy list
void hash_init(void);
void hash_destroy(void);

// Jenkins one-at-a-time hash
uint32_t jenkins_hash(const char *key);

// Operations – these ALSO print the required messages to stdout
void hash_insert(uint32_t hash, const char *name, uint32_t salary);
void hash_delete(uint32_t hash, const char *name);
void hash_update(uint32_t hash, const char *name, uint32_t new_salary);
void hash_search(uint32_t hash, const char *name);
void hash_print_all(void);

#endif
