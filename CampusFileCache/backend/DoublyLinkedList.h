#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include "Node.h"

typedef struct {

    Node *head;
    Node *tail;
    int size;

} DoublyLinkedList;

void initList(DoublyLinkedList *list);

void addFront(DoublyLinkedList *list, Node *node);

void removeNode(DoublyLinkedList *list, Node *node);

Node* removeLRU(DoublyLinkedList *list);

#endif