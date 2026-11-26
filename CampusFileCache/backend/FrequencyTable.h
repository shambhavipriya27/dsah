#ifndef FREQUENCY_TABLE_H
#define FREQUENCY_TABLE_H

#include "DoublyLinkedList.h"

#define FREQ_HASH_SIZE 257

typedef struct {

    int frequency;

    DoublyLinkedList list;

} FrequencyGroup;

typedef struct FrequencyEntry {

    int frequency;

    FrequencyGroup *group;

    struct FrequencyEntry *next;

} FrequencyEntry;

typedef struct {

    FrequencyEntry *buckets[FREQ_HASH_SIZE];

} FrequencyTable;

void initFrequencyTable(FrequencyTable *table);

unsigned int frequencyHash(int frequency);

FrequencyGroup* searchFrequency(
    FrequencyTable *table,
    int frequency
);

FrequencyGroup* createFrequency(
    FrequencyTable *table,
    int frequency
);

void deleteFrequency(
    FrequencyTable *table,
    int frequency
);

#endif