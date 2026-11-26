#include "FrequencyTable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Paste your five existing functions here.
void initFrequencyTable(FrequencyTable *table) {

    int i;

    for (i = 0; i < FREQ_HASH_SIZE; i++) {

        table->buckets[i] = NULL;

    }

}

unsigned int frequencyHash(int frequency) {

    return (unsigned int)frequency % FREQ_HASH_SIZE;

}

FrequencyGroup* searchFrequency(
    FrequencyTable *table,
    int frequency
) {

    unsigned int index = frequencyHash(frequency);

    FrequencyEntry *temp = table->buckets[index];

    while (temp != NULL) {

        if (temp->frequency == frequency) {

            return temp->group;

        }

        temp = temp->next;

    }

    return NULL;

}

FrequencyGroup* createFrequency(
    FrequencyTable *table,
    int frequency
) {

    FrequencyGroup *existing =
        searchFrequency(table, frequency);

    if (existing != NULL) {

        return existing;

    }

    FrequencyGroup *group =
        (FrequencyGroup*)malloc(sizeof(FrequencyGroup));

    if (group == NULL) {

        printf("Memory allocation failed!\n");
        exit(1);

    }

    group->frequency = frequency;

    initList(&group->list);

    FrequencyEntry *entry =
        (FrequencyEntry*)malloc(sizeof(FrequencyEntry));

    if (entry == NULL) {

        free(group);

        printf("Memory allocation failed!\n");
        exit(1);

    }

    entry->frequency = frequency;

    entry->group = group;

    unsigned int index = frequencyHash(frequency);

    entry->next = table->buckets[index];

    table->buckets[index] = entry;

    return group;

}

void deleteFrequency(
    FrequencyTable *table,
    int frequency
) {

    unsigned int index = frequencyHash(frequency);

    FrequencyEntry *temp = table->buckets[index];

    FrequencyEntry *prev = NULL;

    while (temp != NULL) {

        if (temp->frequency == frequency) {

            if (prev == NULL) {

                table->buckets[index] = temp->next;

            }
            else {

                prev->next = temp->next;

            }

            free(temp->group);

            free(temp);

            return;

        }

        prev = temp;

        temp = temp->next;

    }

}