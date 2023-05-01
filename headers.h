#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "DS/queue.h"
#include <math.h>
#include "DS/linkedList.h"

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300
#define BUFF_KEY 450
#define SIM_STATE_KEY 400 // 1 = the process generator is done sending all processes

typedef struct proc_buff
{
    long m_type;
    Process proc;
} PROC_BUFF;

///==============================
// don't mess with this variable//
int *shmaddr; //
//===============================
// 1 = process_generator is done
int *sim_state;

int getClk()
{
    return *shmaddr;
}

// the messsage queue between the process generator and the scheduler, through which processes are sent on their arrival time
int init_buff()
{
    key_t kid = BUFF_KEY;
    int msgid = msgget(kid, 0666 | IPC_CREAT);
    if (msgid == -1)
    {
        perror("Error in msgget\n");
        exit(-1);
    }
    return msgid;
}

// a shared state variable, initiallly 0
// 1 = the process generator has sent all the processes from the input file
// the scheduler reads this variable if the ready queue is empty, if 1 then simulation is over
int init_sim_state()
{
    int shmid = shmget(SIM_STATE_KEY, 4, IPC_CREAT | 0644);
    if (shmid == -1)
    {
        perror("Error in creating sim state!\n");
        exit(-1);
    }
    sim_state = (int *)shmat(shmid, (void *)0, 0);
    if ((long)sim_state == -1)
    {
        perror("Error in attaching sim state!\n");
        exit(-1);
    }
    return shmid;
}

/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
 */
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444|IPC_EXCL);
    while ((int)shmid == -1)
    {
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444|IPC_EXCL);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
 */

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
