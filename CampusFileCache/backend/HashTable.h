#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "Node.h"

#define HASH_SIZE 257

typedef struct {

    Node *buckets[HASH_SIZE];

} HashTable;

void initHashTable(HashTable *table);

unsigned int hashFunction(char *key);

Node* searchHash(HashTable *table, char *key);

void insertHash(HashTable *table, Node *node);

void deleteHash(HashTable *table, char *key);

#endif