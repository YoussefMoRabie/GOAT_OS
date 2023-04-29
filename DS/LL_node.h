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

#ifndef NODE_LL
#define NODE_LL

#include <stdlib.h>
typedef struct LL_Node
{
    int start;
    int end;
} LL_Node;

LL_Node *newNode(int s, int e)
{
    LL_Node *temp = (LL_Node *)malloc(sizeof(LL_Node));
    temp->start = s;
    temp->end = e;
    return temp;
}

#endif