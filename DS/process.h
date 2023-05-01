/**
 * @file process.h
 * @author Yousef Rabia
 * @brief this file contains the process data structure 
 * @version 0.1
 * @date 2023-04-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "LL_node.h"
#ifndef PROCESS_H
#define PROCESS_H

typedef enum pState{
    Ready,
    Running,
}pState;
typedef struct MemBlock{
    int start;
    int end;
    int size;
}MemBlock;

MemBlock* newMemBlock(LL_Node* hole){
    MemBlock *temp = (MemBlock *)malloc(sizeof(MemBlock));
    temp->start = hole->data->start ;
    temp->end = hole ->data ->end;
    temp->size=hole ->data->size;
    return temp;
}
typedef struct Process
{

    int arrivalTime;

    int priority;

    int runningTime;

    int waitingTime;

    int memsize;

    int lastPreempt;

    int remainingTime;

    int lastResume;
    MemBlock memBlock;

    pState p_state;

    int pid;

    int id;

}Process;

#endif