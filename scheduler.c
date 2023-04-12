#include "headers.h"

// typedef enum pState{
//     Ready,
//     Running,
// }pState;

// typedef struct PCB{
//     int pid;
//     pState p_state;
//     int remaining_time;
//     int running_time;
//     int waiting_time;

// } PCB;

// typedef struct process_table{
//     //to be continued
// } process_table;

void Termination_SIG(int signnum);

Process *curProc;
Queue *RQ;

void SRTN(){
    if(curProc->remainingTime > front(RQ)->remainingTime){
        //context switch
        kill(curProc->id,SIGSTOP);
        InsertWithPriority(RQ,curProc,curProc->remainingTime);
        curProc=dequeue(RQ);
    }
    if (curProc->remainingTime==curProc->runningTime)
    {
        //create new process and run it
        int pid=fork();
        if (pid ==0){
            curProc->id=getpid();
            execlp('./proc','./proc',&curProc->remainingTime,NULL);
        }
    }
    else {
        kill(curProc->id,SIGCONT);

    }
}
void terminateCur(){
    curProc=dequeue(RQ);
}
int main(int argc, char * argv[])
{
    initClk();
    RQ= attach_RQ();
    signal(SIGUSR1,Termination_SIG);
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    
    //remove this later
    printf("scheduler is working with algorithm id: %s", argv[1]);
    curProc=dequeue(RQ);
    while(1)
    {
        sleep(100);
    }
    
    destroyClk(true);
}

void Termination_SIG(int signnum){
    terminateCur();
}