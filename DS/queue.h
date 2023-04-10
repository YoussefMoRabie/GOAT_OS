/**
 * @file queue.h
 * @author Yousef Rabia
 * @brief this file contains the queue data structure 
 * @version 0.1
 * @date 2023-04-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "node.h"
#include "process.h"


typedef struct Queue
{
    Node *head;
    Node *tail;
    int size;

} Queue;

Queue *Queue_init(){
    Queue* q;
    q = (struct Queue*) malloc(sizeof(Queue));
    q->size = 0;
    q->head = q->tail = NULL;
    return q;
}

void enqueue(Queue *Q, process *data){
    Node *temp = newNode(data);
    if (Q->tail == NULL) 
    {
        Q->head = Q->tail = temp;
        Q->size += 1;
        return;
    } 
    Q->tail->next = temp;
    Q->tail = temp;
    Q->size += 1;
    return;
}

process *dequeue(Queue *q){
    if (q->size == 0)
        return NULL;
   
    Node *temp = q->head;
    q->head = q->head->next;
    q->size -= 1;

    return temp->data;
}

process *front(Queue *q){
    if (q->size == 0)
        return NULL;
   
    return q->head->data;
}


bool isEmpty(Queue *q){
return (q->size==0);
}

int size_queue(Queue *q){
        return q->size;
}

#endif