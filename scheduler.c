#include "headers.h"

void Termination_SIG(int signnum);
void newProc_arrive(int signum);
void algo_start();
Process *curProc;
Queue *RQ;
int TempCLK;
PROC_BUFF newProc;
int msgBuf;
int algoId;
void (*algo_func)();
bool switched;
bool new_arrive=false;
bool cur_terminated=false;


void SRTN();
void HPF();

int main(int argc, char *argv[])
{
    initClk();

    signal(SIGUSR1, Termination_SIG);
    signal(SIGUSR2, newProc_arrive);

    RQ = Queue_init();
    algoId = atoi(argv[1]);

    msgBuf = init_buff();
    curProc == NULL;

    printf("scheduler is working with algorithm id: %s\n", argv[1]);
    // TODO implement the scheduler :)
    // upon termination release the clock resources.
    // remove this later
    TempCLK = getClk();
    algo_start();

    destroyClk(true);
}

void algo_start(){
    //determine which algorithm to call
    switch (algoId)
    {
    case 1:
        algo_func=&SRTN;
        break;

    default:
        break;
    }
    //looping every clk unit, if a new process arrives or a process terminates
    while (true)
    {
        if (getClk() != TempCLK || new_arrive ||cur_terminated)
        {
            cur_terminated=false;
            TempCLK = getClk();
            // if there is no currently running process nor a ready one
            if (!(isEmpty(RQ) && curProc == NULL))
            {
                algo_func();
            }
        }
    }
}
void SRTN(){

    //if there is no currently process, pick the head of the ready queue
    if (curProc == NULL)
    {
        curProc = dequeue(RQ);
    }
    else
    {   //else you update the current process params

        /* TO BE REMOVED LATER  
        if (curProc->remainingTime==0) 
              return;
        */

        if(!new_arrive) //it doesn't update if a new process arrived cuz it is already updated
        curProc->remainingTime--;
    }
    new_arrive=false; //reset the flag

    if (!isEmpty(RQ) && curProc && curProc->remainingTime > front(RQ)->remainingTime)
    {
        // context switch
        
        switched = true;
        kill(curProc->pid,SIGSTOP);
        curProc->lastPreempt = getClk();
        curProc->p_state = Ready;

        printf("At time %d process %d stopped arr %d total %d remain %d wait %d\n",
               getClk(),
               curProc->id,
               curProc->arrivalTime,
               curProc->runningTime,
               curProc->remainingTime,
               curProc->waitingTime);

        InsertWithPriority(RQ, curProc, curProc->remainingTime);
        curProc = dequeue(RQ);
        curProc->p_state = Running;
    }
    if (curProc &&curProc->remainingTime == curProc->runningTime)
    {
        // create new process and run it
        switched=false;
        int pid = fork();
        if (pid == 0)
        {
            char t[8];
            sprintf(t, "%d", curProc->remainingTime);
            if (execlp("./process.out", "./process.out", t, NULL) == -1)
            {
                perror("error in runnign proc\n");
            }
        }
         curProc->pid = pid;
         curProc->waitingTime += getClk() - curProc->arrivalTime; 
         curProc->p_state = Running;
          printf("At time %d process %d started arr %d total %d remain %d wait %d\n",
                   getClk(),
                   curProc->id,
                   curProc->arrivalTime,
                   curProc->runningTime,
                   curProc->remainingTime,
                   curProc->waitingTime);
    }
    else
    {
        if (switched )
        {   
            //resume the process if there is context switch
            switched=false;
            curProc->waitingTime += getClk() - curProc->lastPreempt;
            printf("At time %d process %d resumed arr %d total %d remain %d wait %d\n",
                   getClk(),
                   curProc->id,
                   curProc->arrivalTime,
                   curProc->runningTime,
                   curProc->remainingTime,
                   curProc->waitingTime);
            kill(curProc->pid, SIGCONT);
        }
    }
}

void HPF(){

}

void Termination_SIG(int signnum)
{
    int TA = getClk() - curProc->arrivalTime;
    float WeTA = (float)TA / curProc->runningTime;

    WTA += WeTA;
    WaitingTime += curProc->waitingTime;
    // print on screen
    printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",
           getClk(),
           curProc->id,
           curProc->arrivalTime,
           curProc->runningTime,
           curProc->remainingTime,
           curProc->waitingTime,
           TA,
           WeTA);
    free(curProc);
    curProc=NULL;
    switched=true;
    //call the algo one more time
    cur_terminated=true;
    // SRTN();
}

void newProc_arrive(int signnum)
{
    new_arrive=true;
    struct msqid_ds stat;
    int i;
    do{
    int rcv = msgrcv(msgBuf, &newProc, sizeof(newProc.proc), 1, !IPC_NOWAIT);
    if (rcv == -1)
    {
        perror("Error in reveive\n");
        exit(-1);
    }
    msgctl(msgBuf, IPC_STAT,&stat);
    i=stat.msg_qnum;
    Process *p = (Process *)malloc(sizeof(Process));
    p->arrivalTime = newProc.proc.arrivalTime;
    p->id = newProc.proc.id;
    p->priority = newProc.proc.priority;
    p->remainingTime = newProc.proc.remainingTime;
    p->runningTime = newProc.proc.runningTime;
    p->waitingTime = newProc.proc.waitingTime;

    // to be modified based on algoId
    // printf("test\n");
    if (algoId != '1') {
      InsertWithPriority(RQ, p, p->remainingTime);
    } else if (algoId != '2') {
      InsertWithPriority(RQ, p, p->priority);

    } else {
      // insertion for RR
    }
    
    } while(i);
    // printf("test %d\n",RQ->head->data->id);

    // SRTN();
}