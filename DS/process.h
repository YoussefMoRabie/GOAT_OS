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
typedef struct process

{

    int arrivalTime;

    int priority;

    int runningTime;

    int remainingTime;

    int id;

    int memSize;

}process;

#endif