#include "DoublyLinkedList.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Paste your four existing functions here.
void initList(DoublyLinkedList *list) {

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

}

void addFront(DoublyLinkedList *list, Node *node) {

    node->prev = NULL;
    node->next = list->head;

    if (list->head != NULL) {

        list->head->prev = node;

    }
    else {

        list->tail = node;

    }

    list->head = node;

    list->size++;

}

void removeNode(DoublyLinkedList *list, Node *node) {

    if (node->prev != NULL) {

        node->prev->next = node->next;

    }
    else {

        list->head = node->next;

    }

    if (node->next != NULL) {

        node->next->prev = node->prev;

    }
    else {

        list->tail = node->prev;

    }

    node->prev = NULL;
    node->next = NULL;

    list->size--;

}

Node* removeLRU(DoublyLinkedList *list) {

    if (list->tail == NULL) {

        return NULL;

    }

    Node *temp = list->tail;

    removeNode(list, temp);

    return temp;

}