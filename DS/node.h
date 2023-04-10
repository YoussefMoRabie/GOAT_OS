/**
 * @file node.h
 * @author Yousef Rabia
 * @brief this file contains the node data structure 
 * @version 0.1
 * @date 2023-04-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef NODE_H
#define NODE_H

#include <stdlib.h>
#include "process.h"
typedef struct Node
{
    process *data;
    struct Node *next;
} Node;

Node *newNode(process *data)
{
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = data;
    temp->next = NULL;
    return temp;
}

#endif