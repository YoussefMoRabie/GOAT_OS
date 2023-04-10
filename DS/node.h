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

typedef struct Node
{
    int *data;
    struct Node *next;
} Node;

#endif