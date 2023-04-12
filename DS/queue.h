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

Queue *Queue_init()
{
    Queue *q;
    q = (struct Queue *)malloc(sizeof(Queue));
    q->size = 0;
    q->head = q->tail = NULL;
    return q;
}

void enqueue(Queue *q, Process *data)
{
    Node *temp = newNode(data,10);
    if (q->tail == NULL)
    {
        q->head = q->tail = temp;
        q->size += 1;
        return;
    }
    q->tail->next = temp;
    q->tail = temp;
    q->size += 1;
    return;
}


void InsertWithPriority(struct Queue *q, struct Process *data, int pri)
{
    Node *temp = newNode(data,pri);
    if (q->tail == NULL)
    {
        q->head = q->tail = temp;
    }
    else 
    {
        if (q->head->priority > temp->priority)
        {
            temp->next = q->head;
            q->head = temp;
        }
        else
        {
            struct Node *next = q->head;
            while (next->next != NULL &&
                   next->next->data->priority < temp->data->priority)
            {
                next = next->next;
            }
            if (next->next != NULL)
            {
                temp->next = next->next;
                next->next = temp;
            }
            else
            {
                next->next = temp;
                q->tail = temp;
            }
        }
    }
    q->size += 1;
    return;
}

Process *dequeue(Queue *q)
{
    if (q->size == 0)
        return NULL;

    Node *temp = q->head;
    q->head = q->head->next;
    q->size -= 1;

    return temp->data;
}

Process *front(Queue *q)
{
    if (q->size == 0)
        return NULL;

    return q->head->data;
}

int isEmpty(Queue *q) { return (q->size == 0); }

int size_queue(Queue *q) { return q->size; }

#endif