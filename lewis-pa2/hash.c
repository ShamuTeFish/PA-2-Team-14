// hash.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

static hashRecord *head = NULL;

void hash_init(void) {
    head = NULL;
}

void hash_destroy(void) {
    hashRecord *curr = head;
    while (curr) {
        hashRecord *next = curr->next;
        free(curr);
        curr = next;
    }
    head = NULL;
}

// Jenkins one at a time hash (32-bit version)
uint32_t jenkins_hash(const char *key) {
    uint32_t hash = 0;
    while (*key) {
        hash += (unsigned char)(*key);
        hash += (hash << 10);
        hash ^= (hash >> 6);
        key++;
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

// Helper: find node + prev by hash
static void find_by_hash(uint32_t hash, hashRecord **out_prev, hashRecord **out_curr) {
    *out_prev = NULL;
    *out_curr = head;
    while (*out_curr && (*out_curr)->hash < hash) {
        *out_prev = *out_curr;
        *out_curr = (*out_curr)->next;
    }
    if (*out_curr && (*out_curr)->hash != hash) {
        // not found
        *out_prev = *out_prev;
        *out_curr = NULL;
    }
}

void hash_insert(uint32_t hash, const char *name, uint32_t salary) {
    hashRecord *prev = NULL;
    hashRecord *curr = head;

    // search correct position (sorted by hash)
    while (curr && curr->hash < hash) {
        prev = curr;
        curr = curr->next;
    }

    if (curr && curr->hash == hash) {
        // duplicate
        printf("Duplicate entry:  Insert failed.  Entry %u is a duplicate.\n", hash);
        return;
    }

    hashRecord *node = malloc(sizeof(hashRecord));
    if (!node) {
        perror("malloc");
        exit(1);
    }
    node->hash = hash;
    strncpy(node->name, name, sizeof(node->name) - 1);
    node->name[sizeof(node->name) - 1] = '\0';
    node->salary = salary;

    // insert into list
    if (prev == NULL) {
        node->next = head;
        head = node;
    } else {
        node->next = prev->next;
        prev->next = node;
    }

    printf("Inserted %u,%s,%u\n", hash, node->name, node->salary);
}

void hash_delete(uint32_t hash, const char *name) {
    hashRecord *prev = NULL;
    hashRecord *curr = head;

    while (curr && curr->hash < hash) {
        prev = curr;
        curr = curr->next;
    }

    if (!curr || curr->hash != hash) {
        printf("Entry %u not deleted.  Not in database.\n", hash);
        return;
    }

    if (prev == NULL) {
        head = curr->next;
    } else {
        prev->next = curr->next;
    }

    printf("Deleted record for %u,%s,%u\n", curr->hash, curr->name, curr->salary);
    free(curr);
}

void hash_update(uint32_t hash, const char *name, uint32_t new_salary) {
    hashRecord *curr = head;

    while (curr && curr->hash < hash) {
        curr = curr->next;
    }

    if (!curr || curr->hash != hash) {
        printf("Update failed. Entry %u not found.\n", hash);
        return;
    }

    uint32_t old_salary = curr->salary;
    curr->salary = new_salary;

    printf("Updated record %u from %u,%s,%u to %u,%s,%u\n",
           hash,
           hash, curr->name, old_salary,
           hash, curr->name, curr->salary);
}

void hash_search(uint32_t hash, const char *search_name) {
    hashRecord *curr = head;

    while (curr && curr->hash < hash) {
        curr = curr->next;
    }

    if (!curr || curr->hash != hash) {
        printf("%s not found.\n", search_name);
        return;
    }

    printf("Found: %u,%s,%u\n", curr->hash, curr->name, curr->salary);
}

void hash_print_all(void) {
    printf("Current Database:\n");
    hashRecord *curr = head;
    while (curr) {
        printf("%u,%s,%u\n", curr->hash, curr->name, curr->salary);
        curr = curr->next;
    }
}
