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

#ifndef PROCESS_H
#define PROCESS_H

typedef enum pState{
    Ready,
    Running,
}pState;

typedef struct Process
{

    int arrivalTime;

    int priority;

    int runningTime;

    int waitingTime;

    int lastPreempt;

    int remainingTime;

    pState p_state;

    int pid;

    int id;

}Process;

#endif