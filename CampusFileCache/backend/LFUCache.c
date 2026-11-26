#include "LFUCache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Paste your six existing functions here.
void initCache(LFUCache *cache, int capacity) {

    cache->capacity = capacity;

    cache->size = 0;

    cache->minFreq = 0;

    initHashTable(&cache->cache);

    initFrequencyTable(&cache->frequencies);

}

void increaseFrequency(LFUCache *cache, Node *node) {

    int oldFreq = node->freq;

    // Find the old frequency group
    FrequencyGroup *oldGroup =
        searchFrequency(&cache->frequencies, oldFreq);

    // Remove node from old group
    removeNode(&oldGroup->list, node);

    // Check if old group became empty
    if (oldGroup->list.size == 0) {

        deleteFrequency(&cache->frequencies, oldFreq);

        // Update minimum frequency
        if (cache->minFreq == oldFreq) {
            cache->minFreq = oldFreq + 1;
        }
    }

    // Increase frequency
    node->freq++;

    // Find or create new frequency group
    FrequencyGroup *newGroup =
        createFrequency(&cache->frequencies, node->freq);

    // Insert at front (most recently used)
    addFront(&newGroup->list, node);
}

int GET(LFUCache *cache, char *key, char *result) {

    // Search in hash table
    Node *node = searchHash(&cache->cache, key);

    // Cache miss
    if (node == NULL) {

        return 0;
    }

    // Copy saved value
    strcpy(result, node->value);

    // Increase access frequency
    increaseFrequency(cache, node);

    return 1;
}

int PUTDetailed(LFUCache *cache, char *key, char *value,
                char *evictedKey, int *evictedFreq, int *tieCount) {
    Node *existing = searchHash(&cache->cache, key);
    if (evictedKey) evictedKey[0] = '\0';
    if (evictedFreq) *evictedFreq = 0;
    if (tieCount) *tieCount = 0;

    if (existing != NULL) {
        strcpy(existing->value, value);
        increaseFrequency(cache, existing);
        return 2; /* updated */
    }
    if (cache->capacity == 0) return 0; /* rejected: zero capacity */

    /* Allocate before eviction so an allocation failure never loses a cached file. */
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) return -1;
    strcpy(newNode->key, key);
    strcpy(newNode->value, value);
    newNode->freq = 1;
    newNode->prev = newNode->next = newNode->hashNext = NULL;

    if (cache->size == cache->capacity) {
        FrequencyGroup *group = searchFrequency(&cache->frequencies, cache->minFreq);
        if (group == NULL || group->list.tail == NULL) {
            free(newNode);
            return -1;
        }
        int oldMin = cache->minFreq;
        int ties = group->list.size;
        Node *victim = removeLRU(&group->list);
        if (evictedKey) strcpy(evictedKey, victim->key);
        if (evictedFreq) *evictedFreq = victim->freq;
        if (tieCount) *tieCount = ties;
        deleteHash(&cache->cache, victim->key);
        cache->size--;
        if (group->list.size == 0) deleteFrequency(&cache->frequencies, oldMin);
        free(victim);
    }

    insertHash(&cache->cache, newNode);
    FrequencyGroup *one = createFrequency(&cache->frequencies, 1);
    addFront(&one->list, newNode);
    cache->size++;
    cache->minFreq = 1;
    return 1; /* inserted */
}

void PUT(LFUCache *cache, char *key, char *value) {
    char evictedKey[MAX_KEY];
    int evictedFreq = 0, tieCount = 0;
    int result = PUTDetailed(cache, key, value, evictedKey, &evictedFreq, &tieCount);
    if (result == 1) printf("Inserted %s successfully.\n", key);
    else if (result == 2) printf("Updated %s successfully.\n", key);
    else if (result == 0) printf("Cannot insert: Cache capacity is zero.\n");
    else printf("PUT failed.\n");
    if (evictedKey[0]) {
        printf("Eviction: %s (frequency %d).\n", evictedKey, evictedFreq);
        printf("Reason: %s\n", tieCount > 1 ? "LFU tie; least recently used file removed." : "Unique lowest frequency.");
    }
}

void STATE(LFUCache *cache) {

    if (cache->size == 0) {

        printf("\nCache is empty!\n");

        return;
    }

    // Maximum number of groups cannot exceed cache capacity
    FrequencyGroup *groups[MAX_CAPACITY];

    int count = 0;

    // Collect all existing frequency groups
    for (int i = 0; i < FREQ_HASH_SIZE; i++) {

        FrequencyEntry *temp =
            cache->frequencies.buckets[i];

        while (temp != NULL) {

            groups[count++] = temp->group;

            temp = temp->next;
        }
    }

    // Sort frequency groups in ascending order
    for (int i = 1; i < count; i++) {

        FrequencyGroup *key = groups[i];

        int j = i - 1;

        while (j >= 0 &&
               groups[j]->frequency > key->frequency) {

            groups[j + 1] = groups[j];

            j--;
        }

        groups[j + 1] = key;
    }

    printf("\n========== CACHE STATE ==========\n");

    printf("Capacity: %d\n", cache->capacity);

    printf("Current Size: %d\n", cache->size);

    printf("Minimum Frequency: %d\n", cache->minFreq);

    printf("\nKey\tValue\tFrequency\n");

    // Display each frequency group
    for (int i = 0; i < count; i++) {

        FrequencyGroup *group = groups[i];

        printf("\nFrequency %d:\n", group->frequency);

        Node *temp = group->list.head;

        while (temp != NULL) {

            printf("%s\t%s\t%d\n",
                   temp->key,
                   temp->value,
                   temp->freq);

            temp = temp->next;
        }
    }

    printf("=================================\n");
}

void destroyCache(LFUCache *cache) {

    // Free all file nodes
    for (int i = 0; i < HASH_SIZE; i++) {

        Node *temp = cache->cache.buckets[i];

        while (temp != NULL) {

            Node *nextNode = temp->hashNext;

            free(temp);

            temp = nextNode;
        }

        cache->cache.buckets[i] = NULL;
    }

    // Free all frequency groups and entries
    for (int i = 0; i < FREQ_HASH_SIZE; i++) {

        FrequencyEntry *temp =
            cache->frequencies.buckets[i];

        while (temp != NULL) {

            FrequencyEntry *nextEntry = temp->next;

            free(temp->group);

            free(temp);

            temp = nextEntry;
        }

        cache->frequencies.buckets[i] = NULL;
    }

    cache->size = 0;

}