#ifndef LFU_CACHE_H
#define LFU_CACHE_H

#include "HashTable.h"
#include "FrequencyTable.h"

#define MAX_CAPACITY 200
#define MAX_OPERATIONS 12000

typedef struct {

    int capacity;
    int size;
    int minFreq;

    HashTable cache;

    FrequencyTable frequencies;

} LFUCache;

void initCache(LFUCache *cache, int capacity);

void increaseFrequency(LFUCache *cache, Node *node);

int GET(LFUCache *cache, char *key, char *result);

void PUT(LFUCache *cache, char *key, char *value);

int PUTDetailed(LFUCache *cache, char *key, char *value,
                char *evictedKey, int *evictedFreq, int *tieCount);

void STATE(LFUCache *cache);

void destroyCache(LFUCache *cache);

#endif