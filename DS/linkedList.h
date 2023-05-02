/**
 * @file linkedList.h
 * @author Yousef Rabia
 * @brief this file contains the linkedList data structure
 * @version 0.1
 * @date 2023-05-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "LL_node.h"
#include "hole.h"

typedef struct LinkedList
{
    LL_Node *head;
    int size;

} LinkedList;

LinkedList *LinkedList_init()
{
    LinkedList *ll;
    ll = (struct LinkedList *)malloc(sizeof(LinkedList));
    ll->size = 0;
    ll->head  = NULL;
    return ll;
}

// void insertByHole(LinkedList *ll, Hole *data)
// {
//     LL_Node *temp = newNode(data->start,data->end);
//     temp->priority=pri;
//     temp->next=ll->head;
//     ll->head = temp; 
//     ll->size += 1;
//     return;
// }



// void InsertHoleWithPriority(LinkedList *ll, Hole *data,int pri)
// {
//     LL_Node *temp = newNode(data->start,data->end);
//     temp->priority=pri;
//     if (ll->head == NULL||ll->head->priority>pri)
//     {
//         temp->next=ll->head;
//         ll->head = temp;
//     }
//     else 
//     {
//             struct LL_Node *next = ll->head;
//             while (next->next != NULL &&
//                    next->next->priority <= temp->priority)
//             {
//                 next = next->next;
//             }
//             if (next->next != NULL)
//             {
//                 temp->next = next->next;
//                 next->next = temp;
//             }
//             else
//             {
//                 next->next = temp;
//             }
        
//     }
//     ll->size += 1;
//     return;
// }

LL_Node * insertByStartAndEnd(LinkedList *ll, int start,int end)
{
    printf("here1...\n");
    LL_Node *temp = newLLNode(start,end);
    if (ll->head == NULL||ll->head->data->start>start)
    {
        printf("here2...\n");
        temp->next=ll->head;
        ll->head->prev=temp;
        ll->head = temp;
    }
    else
    {
            struct LL_Node *next = ll->head;
            while (next->next != NULL &&
                   next->next->data->start <= temp->data->start)
            {
                next = next->next;
            }
            if (next->next != NULL)
            {
                temp->next = next->next;
                next->next->prev = temp;
                next->next = temp;
                temp->prev=next;
            }
            else
            {
                next->next = temp;
                temp->prev=next;
            }
    }
    
    ll->size += 1;
///////////////////////////////////////////delete////////////////////
    struct LL_Node *next = ll->head;
    while(next!= NULL)
    {
    printf("%p     %p(%d,%d)      %p \n",next->prev,next,next->data->start,next->data->start,next->next);
    next = next->next;
    }
    
//////////////////////////////////////////////////////////////////////////////////////////////////

    return temp;
}

void removeNode(LinkedList *ll,LL_Node *  node){
    if(node->prev==NULL)
    {
        ll->head=ll->head->next;
        ll->head->prev=NULL;
    }else{
        node->prev->next=node->next;
        node->next->prev=node->prev;
    }

    //delete node;

    //node=NULL;
}
int isLLEmpty(LinkedList *ll) { return (ll->size == 0); }

int size_LL(LinkedList *ll) { return ll->size; }

#endif