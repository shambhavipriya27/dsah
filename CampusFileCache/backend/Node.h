#ifndef NODE_H
#define NODE_H

#define MAX_KEY 64
#define MAX_VALUE 256

typedef struct Node {

    char key[MAX_KEY];
    char value[MAX_VALUE];

    int freq;

    struct Node *prev;
    struct Node *next;
    struct Node *hashNext;

} Node;

#endif