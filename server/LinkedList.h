#include <stdio.h>
#include <stdlib.h>

struct NodeData {
    pthread_t tid;
    int cfd;
    bool isDone;
};

// node creation
struct Node {
    struct NodeData data;
    struct Node* next;
    struct Node* prev;
};

struct Node* appendNode(struct Node** head, struct NodeData data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));

    newNode->data = data;
    newNode->next = NULL;
    struct Node* temp = *head;

    if (*head == NULL) {
        newNode->prev = NULL;
        *head = newNode;
        return newNode;
    }

    while (temp->next != NULL) temp = temp->next;

    temp->next = newNode;

    newNode->prev = temp;

    return newNode;
}

void removeNode(struct Node** head, struct Node* del_node) {
    if (*head == NULL || del_node == NULL) {
        return;
    }

    if (*head == del_node) *head = del_node->next;
    if (del_node->next != NULL) del_node->next->prev = del_node->prev;
    if (del_node->prev != NULL) del_node->prev->next = del_node->next;

    free(del_node);
}

static void joinDoneThread(struct Node** head) {
    struct Node* curr = *head;
    while (curr) {
        struct Node* tmp = curr->next;
        if (curr->data.isDone) {
            pthread_join(curr->data.tid, NULL);
            removeNode(head, curr);
        }
        curr = tmp;
    }
}

void destroyList(struct Node** head) {
    struct Node* curr = *head;
    while (curr) {
        struct Node* tmp = curr->next;

        pthread_join(curr->data.tid, NULL);
        removeNode(head, curr);

        curr = tmp;
    }
}