
#ifndef OS_HW3_QUEUE_H
#define OS_HW3_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
typedef struct node{
    int fd;
    struct timeval arrival_time;
    struct node* next;
}*Node ;

typedef struct queue{
    Node head;
    Node tail;
    int size;
} *Queue ;

Node CreateNode(int fd);
Queue CreateQueue();
void Enqueue(Queue queue, int fd);
void DestroyNode(Node node);
void Dequeue(Queue queue);
void removeNode(Queue queue,int fd);
int DequeueIndex(Queue queue, int index);
#endif //OS_HW3_QUEUE_H
