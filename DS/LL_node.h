/**
 * @file node.h
 * @author Yousef Rabia
 * @brief this file contains the LL node data structure 
 * @version 0.1
 * @date 2023-05-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef NODE_LL
#define NODE_LL

#include <stdlib.h>
#include "hole.h"

typedef struct LL_Node
{
    Hole *data;
    struct LL_Node *prev;
    struct LL_Node *next;
} LL_Node;

LL_Node *newNode(int start, int end)
{
    LL_Node *temp = (LL_Node *)malloc(sizeof(LL_Node));
    temp->data->start = start;
    temp->data->end = end;
    temp->next=temp->prev=NULL;
    return temp;
}

#endif