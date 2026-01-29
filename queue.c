#include <stdio.h>
#include "queue.h"
#include <stdlib.h>




Node CreateNode(int fd){
    Node node = (Node)malloc(sizeof(*node));
    node->fd = fd;
    struct timeval arr_time;
    gettimeofday(&(arr_time),NULL);
    node->arrival_time = arr_time;
    node->next = NULL;
    return node;
}
Queue CreateQueue(){
    Queue queue = malloc(sizeof (*queue));
    queue->head=NULL;
    queue->tail=NULL;
    queue->size = 0;
    return queue;
}
void Enqueue(Queue queue, int fd){
   Node node = CreateNode(fd);
   if(queue->head == NULL){
       queue->head = node;
       queue->tail=node;
       queue->size++;
   }
   else{
       queue->tail->next = node;
       queue->tail = node;
       queue->size++;
   }

}

void DestroyNode(Node node){
    free(node);
}

void Dequeue(Queue queue){
    if(queue->size == 0){
        return;
    }
    Node temp = queue->head->next;
    DestroyNode(queue->head);
    if(temp == NULL){
      queue->head = NULL;
      queue ->tail = NULL;
      queue->size --;
      return;
    }
    queue->head = temp;
    queue->size --;
}
void removeNode(Queue queue,int fd){

    if (queue->size == 0){
        return;
    }
    if (queue->head->fd==fd){
        if(queue->head->next == NULL){
            DestroyNode(queue->head);
            queue->head = NULL;
            queue->tail = NULL;
            queue->size--;
            return;
        }
        Node next =queue->head->next;
        DestroyNode(queue->head);
        queue->head = next;
        queue->size--;
        return;
    }
    else{
        Node prev_node= queue->head;
        Node temp_node = queue->head->next;
        while ( temp_node != NULL){
            if(temp_node->fd==fd){
                if(temp_node->fd == queue->tail->fd){
                    queue->tail = prev_node;
                }
                prev_node->next=temp_node->next;
                DestroyNode(temp_node);
                queue->size--;
                return;
            }
            prev_node=temp_node;
            temp_node = temp_node->next;
        }
    }
}
int DequeueIndex(Queue queue, int index){

    if(index==0){
        int head_fd = queue->head->fd;
        Dequeue(queue);
        return head_fd;
    }
    Node temp = queue->head;
    for(int i=0; i < index; i++){
        temp=temp->next;
    }
    int temp_fd = temp->fd;
    removeNode(queue,temp_fd);
    return temp_fd;
}