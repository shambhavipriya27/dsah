#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LFUCache.h"
#include "ServerMode.h"

int main(int argc, char **argv) {

    if (argc > 1 && strcmp(argv[1], "--server") == 0) {
        return runServerMode();
    }

    LFUCache cache;

    int capacity;
    int operations;

    char command[20];
    char key[MAX_KEY];
    char value[MAX_VALUE];
    char result[MAX_VALUE];

    printf("===== CAMPUS FILE CACHE SIMULATOR =====\n");

    printf("Enter cache capacity (0-200): ");

    if (scanf("%d", &capacity) != 1) {
        printf("Invalid input!\n");
        return 1;
    }

    if (capacity < 0 || capacity > MAX_CAPACITY) {

        printf("Invalid capacity!\n");

        return 1;
    }

    initCache(&cache, capacity);

    printf("Enter number of operations (0-12000): ");

    if (scanf("%d", &operations) != 1) {

        printf("Invalid input!\n");

        destroyCache(&cache);

        return 1;
    }

    if (operations < 0 || operations > MAX_OPERATIONS) {

        printf("Invalid number of operations!\n");

        destroyCache(&cache);

        return 1;
    }

    printf("\nEnter operations:\n");

    for (int i = 1; i <= operations; i++) {

        if (scanf("%19s", command) != 1) {
            printf("Invalid input.\n");
            break;
        }

        printf("\n--- Operation %d ---\n", i);

        // GET operation
        if (strcmp(command, "GET") == 0) {

            if (scanf("%63s", key) != 1) {
                printf("Invalid GET input.\n");
                break;
            }

            if (GET(&cache, key, result)) {

                printf("GET(%s) -> %s\n", key, result);

            }
            else {

                printf("GET(%s) -> CACHE MISS\n", key);

            }
        }

        // PUT operation
        else if (strcmp(command, "PUT") == 0) {

            if (scanf("%63s %255s", key, value) != 2) {
                printf("Invalid PUT input.\n");
                break;
            }

            printf("PUT(%s, %s)\n", key, value);

            PUT(&cache, key, value);

        }

        // STATE operation
        else if (strcmp(command, "STATE") == 0) {

            STATE(&cache);

        }

        else {

            printf("Unknown operation: %s\n", command);

        }
    }

    // Release allocated memory
    destroyCache(&cache);

    printf("\nProgram completed successfully.\n");

    return 0;
}
