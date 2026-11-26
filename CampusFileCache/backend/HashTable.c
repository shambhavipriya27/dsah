#include "HashTable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Paste your five existing functions here.
void initHashTable(HashTable *table) {

    int i;

    for (i = 0; i < HASH_SIZE; i++) {

        table->buckets[i] = NULL;

    }

}

unsigned int hashFunction(char *key) {

    unsigned int hash = 0;

    while (*key != '\0') {

        hash = hash * 31 + (unsigned char)*key;

        key++;

    }

    return hash % HASH_SIZE;

}

Node* searchHash(HashTable *table, char *key) {

    unsigned int index = hashFunction(key);

    Node *temp = table->buckets[index];

    while (temp != NULL) {

        if (strcmp(temp->key, key) == 0) {

            return temp;

        }

        temp = temp->hashNext;

    }

    return NULL;

}

void insertHash(HashTable *table, Node *node) {

    unsigned int index = hashFunction(node->key);

    node->hashNext = table->buckets[index];

    table->buckets[index] = node;

}

void deleteHash(HashTable *table, char *key) {

    unsigned int index = hashFunction(key);

    Node *temp = table->buckets[index];

    Node *prev = NULL;

    while (temp != NULL) {

        if (strcmp(temp->key, key) == 0) {

            if (prev == NULL) {

                table->buckets[index] = temp->hashNext;

            }
            else {

                prev->hashNext = temp->hashNext;

            }

            temp->hashNext = NULL;

            return;

        }

        prev = temp;

        temp = temp->hashNext;

    }

}