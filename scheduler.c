#include "headers.h"

void Termination_SIG(int signnum);
void newProc_arrive(int signum);
void algo_start();
void output_performance();
void write_in_logFile_start();
void write_in_logFile_resume();
void write_in_logFile_stop();

Process *curProc;
Queue *RQ;  //ready queue
Queue *WQ;  //queue for processes unable to initialize because of not having enough memory
int TempCLK;
PROC_BUFF newProc;
int msgBuf;
int algoId;
int mem_algo_id;
void (*algo_func)();
bool switched;
bool new_arrive = false;
bool cur_terminated = false;
int Quantum; // input from user when they choose round robin
// variables for the output files
FILE *log_file;
FILE *perf;
int process_count;
int tot_Wait = 0;
int tot_runtime = 0;
float *WTA_ARR; // array of WTA, used in scheduler.log math
int WTA_INDEX = 0;


void Buddy_Init();
//Memory Lists
LinkedList* All_Holes[9];
LinkedList *Holes_2;
LinkedList *Holes_1;
LinkedList *Holes_4;
LinkedList *Holes_8;
LinkedList *Holes_16;
LinkedList *Holes_32;
LinkedList *Holes_64;
LinkedList *Holes_128;
LinkedList *Holes_256;

void SRTN();
void HPF();
void RR();

int main(int argc, char *argv[])
{
    initClk();
    init_sim_state();

    Buddy_Init();
    signal(SIGUSR1, Termination_SIG);
    signal(SIGUSR2, newProc_arrive);

    RQ = Queue_init();
    WQ = Queue_init(); 
    algoId = atoi(argv[1]);
    mem_algo_id = atoi(argv[2]);
    // for round robin
    if (algoId == 3)
    {
        Quantum = atoi(argv[4]);
    }

    msgBuf = init_buff();
    curProc == NULL;

    process_count = atoi(argv[3]);
    WTA_ARR = malloc(process_count * sizeof(float));

    perf = fopen("scheduler.log", "w");
    fprintf(perf, "#At time x process y state arr w total z remain y wait k\n");
    fclose(perf);

    printf("scheduler is working with algorithm id: %d and memory algorithm: %d\n", algoId, mem_algo_id);
    // TODO implement the scheduler :)
    // upon termination release the clock resources.
    TempCLK = getClk();
    algo_start();
}

void algo_start()
{
    // determine which algorithm to call
    switch (algoId)
    {
    case 1:
        algo_func = &SRTN;
        break;
    case 2:
        algo_func = &HPF;
        break;
    case 3:
        algo_func = &RR;
        break;
    default:
        break;
    }
    // looping every clk unit, if a new process arrives or a process terminates
    while (true)
    {
        if (getClk() != TempCLK || new_arrive || cur_terminated)
        {
            cur_terminated = false;
            TempCLK = getClk();
            // if there is no currently running process nor a ready one
            if (!(isEmpty(RQ) && curProc == NULL))
            {
                algo_func();
            }
            else if (isEmpty(RQ) && *sim_state == 1) // ready queue is empty, and process generator is done sending
            {
                // outputting "scheduler.perf" and then ending the simulation
                output_performance();
                destroyClk(true);
            }
        }
    }
}

void SRTN()
{
    // if there is no currently process, pick the head of the ready queue
    if (curProc == NULL)
    {
        curProc = dequeue(RQ);
    }
    else if (!new_arrive)
    {
        // it doesn't update if a new process arrived cuz it is already updated
        curProc->remainingTime--;
    }
    new_arrive = false; // reset the flag

    if (!isEmpty(RQ) && curProc && curProc->remainingTime > front(RQ)->remainingTime)
    {
        // context switch

        switched = true;
        kill(curProc->pid, SIGSTOP);
        curProc->lastPreempt = getClk();
        curProc->p_state = Ready;

        write_in_logFile_stop();

        InsertWithPriority(RQ, curProc, curProc->remainingTime);
        curProc = dequeue(RQ);
        curProc->p_state = Running;
    }

    if (curProc && curProc->remainingTime == curProc->runningTime)
    {
        // create new process and run it
        switched = false;

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

        // output to the scheduler.log file
        write_in_logFile_start();
    }
    else
    {
        if (switched)
        {
            // resume the process if there is context switch
            switched = false;
            curProc->waitingTime += getClk() - curProc->lastPreempt;

            // output to the scheduler.log file
            write_in_logFile_resume();

            kill(curProc->pid, SIGCONT);
        }
    }
}

void HPF()
{
    if (new_arrive && curProc != NULL)
    {
        new_arrive = false;
        return;
    }
    // if there is no currently process, pick the head of the ready queue
    if (curProc == NULL)
    {
        curProc = dequeue(RQ);
    }
    else
    { // else you update the current process params
        curProc->remainingTime--;
    }

    if (curProc && curProc->remainingTime == curProc->runningTime)
    {
        // create new process and run it
        switched = false;
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

        // output to the scheduler.log file
        write_in_logFile_start();
    }
    else if (switched)
    {
        // resume the process if there is context switch
        switched = false;
        curProc->waitingTime += getClk() - curProc->lastPreempt;

        // output to the scheduler.log file
        write_in_logFile_resume();

        kill(curProc->pid, SIGCONT);
    }
}
void RR()
{
    // if there is no currently process, pick the head of the ready queue
    if (curProc == NULL)
    {
        curProc = dequeue(RQ);
    }
    else
    {
        if (!new_arrive) // it doesn't update if a new process arrived cuz it is already updated
            curProc->remainingTime--;
    }
    new_arrive = false; // reset the flag
    if (!isEmpty(RQ) && curProc && getClk() - curProc->lastResume == Quantum && curProc->remainingTime > 0)
    {
        // context switch
        switched = true;
        printf("sch %d remain: %d\n", curProc->pid, curProc->remainingTime);
        kill(curProc->pid, SIGSTOP);
        curProc->lastPreempt = getClk();
        curProc->p_state = Ready;

        // output to the scheduler.log file
        write_in_logFile_stop();

        enqueue(RQ, curProc);
        curProc = dequeue(RQ);
        curProc->p_state = Running;
    }
    if (curProc && curProc->remainingTime == curProc->runningTime)
    {
        // create new process and run it
        switched = false;
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
        curProc->lastResume = getClk();

        // output to the scheduler.log file
        write_in_logFile_start();
    }
    else
    {
        if (switched)
        {
            // resume the process if there is context switch
            switched = false;
            curProc->waitingTime += getClk() - curProc->lastPreempt;
            curProc->lastResume = getClk();

            // output to the scheduler.log file
            write_in_logFile_resume();

            kill(curProc->pid, SIGCONT);
        }
    }
}

bool First_Fit()
{
    return true;
}
void Buddy_Init(){
    Holes_1=LinkedList_init();
    All_Holes[0]=Holes_1;
    Holes_2=LinkedList_init();
    All_Holes[1]=Holes_2;
    Holes_4=LinkedList_init();
    All_Holes[2]=Holes_4;
    Holes_8=LinkedList_init();
    All_Holes[3]=Holes_8;
    Holes_16=LinkedList_init();
    All_Holes[4]=Holes_16;
    Holes_32=LinkedList_init();
    All_Holes[5]=Holes_32;
    Holes_64=LinkedList_init();
    All_Holes[6]=Holes_64;
    Holes_128=LinkedList_init();
    All_Holes[7]=Holes_128;
    Holes_256=LinkedList_init();

    LL_Node *hole256_1=newLLNode(768,1023);
    hole256_1->next=NULL;
        printf("after \n ");
    LL_Node *hole256_2=newLLNode(512,767);
    hole256_2->next=hole256_1;
    LL_Node *hole256_3=newLLNode(256,511);
    hole256_3->next=hole256_2;
    LL_Node *hole256_4=newLLNode(0,255);
    hole256_4->next=hole256_3;

    Holes_256->size=4;
    Holes_256->head=hole256_4;
    All_Holes[8]=Holes_256;
    printf("All Size %d\n",All_Holes[8]->size);
}
void Partition(int cur_hole_idx,int needed_hole_idx){
    printf("Entered Partition\n");
    while (cur_hole_idx != needed_hole_idx){
        LL_Node* toSplit =All_Holes[cur_hole_idx]->head;
        All_Holes[cur_hole_idx]->head=All_Holes[cur_hole_idx]->head->next;
        All_Holes[cur_hole_idx]->size-=1;
        int newSize=toSplit->data->size/2;
        LL_Node* new_2=newLLNode(toSplit->data->start+ newSize,toSplit->data->end);
        new_2->next=All_Holes[cur_hole_idx-1]->head;
        LL_Node* new_1=newLLNode(toSplit->data->start,new_2->data->start-1);
        new_1->next=new_2;
        All_Holes[cur_hole_idx-1]->head=new_1;
        All_Holes[cur_hole_idx-1]->size+=2;
        cur_hole_idx--;
    }

}
bool Buddy(Process* proc)
{
    printf("Entered Buddy\n");
     int Original_Size=proc->memsize;
    int Needed_Hole_idx=ceil(log2(Original_Size));
    int cur_hole_idx=Needed_Hole_idx;
    printf("cur idxx %d,Needed %d, %d\n",cur_hole_idx,Needed_Hole_idx,All_Holes[cur_hole_idx]->size);

    while (cur_hole_idx<9 && isLLEmpty(All_Holes[cur_hole_idx])){

        cur_hole_idx++;
    }
    printf("cur idxx %d,Needed %d\n",cur_hole_idx,Needed_Hole_idx);
    if(cur_hole_idx == 9)
        return false;
    if(cur_hole_idx!=Needed_Hole_idx)
        Partition(cur_hole_idx,Needed_Hole_idx);

    MemBlock* Allocated_Block=newMemBlock(All_Holes[Needed_Hole_idx]->head);
    printf("Procces %d allocated space from %d to %d of size %d\n",proc->id,Allocated_Block->start,Allocated_Block->end,Allocated_Block->size);
    LL_Node* temp =All_Holes[Needed_Hole_idx]->head;
    All_Holes[Needed_Hole_idx]->head=All_Holes[Needed_Hole_idx]->head->next;
    All_Holes[Needed_Hole_idx]->size-=1;
    //  for (int i=0; i<9; i++){
    //     if (All_Holes[i]->Head!=NULL){
    //         printf("%d ----> %d\n",i,All_Holes[i]->Head->start);
    //     }
    // }
    free(temp);
    return true;
}

void Termination_SIG(int signnum)
{
    int TA = getClk() - curProc->arrivalTime;
    float WeTA = (float)TA / curProc->runningTime;

    tot_Wait += curProc->waitingTime;
    tot_runtime += curProc->runningTime;
    WTA_ARR[WTA_INDEX] = WeTA;
    WTA_INDEX++;

    // output to the "scheduler.log" file
    log_file = fopen("scheduler.log", "a");
    fprintf(log_file, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",
            getClk(),
            curProc->id,
            curProc->arrivalTime,
            curProc->runningTime,
            curProc->remainingTime,
            curProc->waitingTime,
            TA,
            WeTA);
    fclose(log_file);

    free(curProc);
    curProc = NULL;
    switched = true;
    cur_terminated = true;

    while(!isEmpty(WQ))
    {
    if(mem_algo_id == 1)
    {
        if(First_Fit(front(WQ)))
        {
            Process* p = dequeue(WQ);
            if (algoId == 1)
            {
                InsertWithPriority(RQ, p, p->remainingTime);
            }
            else if (algoId == 2)
            {
                InsertWithPriority(RQ, p, p->priority);
            }
            else if (algoId == 3)
            {
                enqueue(RQ, p);
            }

        }
    }
    else
    {
        if(Buddy(front(WQ)))
        {
            Process* p = dequeue(WQ);
            if (algoId == 1)
            {
                InsertWithPriority(RQ, p, p->remainingTime);
            }
            else if (algoId == 2)
            {
                InsertWithPriority(RQ, p, p->priority);
            }
            else if (algoId == 3)
            {
                enqueue(RQ, p);
            }
        }
    }
    }
}

void newProc_arrive(int signnum)
{
    struct msqid_ds stat;
    int i;
    do
    {
        int rcv = msgrcv(msgBuf, &newProc, sizeof(newProc.proc), 1, !IPC_NOWAIT);
        if (rcv == -1)
        {
            perror("Error in receive\n");
            exit(-1);
        }
        msgctl(msgBuf, IPC_STAT, &stat);
        i = stat.msg_qnum;
        Process *p = (Process *)malloc(sizeof(Process));
        p->arrivalTime = newProc.proc.arrivalTime;
        p->id = newProc.proc.id;
        p->priority = newProc.proc.priority;
        p->remainingTime = newProc.proc.remainingTime;
        p->runningTime = newProc.proc.runningTime;
        p->waitingTime = newProc.proc.waitingTime;
        p->memsize = newProc.proc.memsize;

        bool allocation_result = false;
        if (mem_algo_id == 1)
        {
            allocation_result = First_Fit();
        }
        else
        {
            allocation_result = Buddy(p);
        }
        // to be modified based on algoId
        if (allocation_result)
        {
            if (algoId == 1)
            {
                InsertWithPriority(RQ, p, p->remainingTime);
            }
            else if (algoId == 2)
            {
                InsertWithPriority(RQ, p, p->priority);
            }
            else if (algoId == 3)
            {
                enqueue(RQ, p);
            }
        }
        else
        {
            enqueue(WQ, p);
        }
    } while (i);
    new_arrive = true;
}

// outputs the scheduler.perf at the end of the simulation
void output_performance()
{
    int curr_time = getClk();
    float CPU_utilization = (tot_runtime / (float)curr_time) * 100;
    float avg_wait = (float)tot_Wait / process_count;
    float WTA_SUM = 0;
    float sum_squared_error = 0;
    for (int i = 0; i < process_count; i++)
    {
        WTA_SUM += WTA_ARR[i];
    }
    float WTA_AVG = WTA_SUM / process_count;
    for (int i = 0; i < process_count; i++)
    {
        sum_squared_error += pow((WTA_ARR[i] - WTA_AVG), 2);
    }
    float WTA_STD = sqrt(sum_squared_error / process_count);

    printf("proc_count: %d curr_time: %d tot_runtime: %d tot_wait: %d\n", process_count, curr_time, tot_runtime, tot_Wait);
    perf = fopen("scheduler.perf", "w");
    fprintf(perf, "CPU utilization = %f\n", CPU_utilization);
    fprintf(perf, "Avg WTA = %f\n", WTA_AVG);
    fprintf(perf, "Avg Waiting = %f\n", avg_wait);
    fprintf(perf, "Std WTA = %f\n", WTA_STD);
    fclose(perf);
}

void write_in_logFile_start()
{
    // output to the scheduler.log file
    log_file = fopen("scheduler.log", "a");
    fprintf(log_file, "At time %d process %d started arr %d total %d remain %d wait %d\n",
            getClk(),
            curProc->id,
            curProc->arrivalTime,
            curProc->runningTime,
            curProc->remainingTime,
            curProc->waitingTime);
    fclose(log_file);
}
void write_in_logFile_stop()
{
    // output to the scheduler.log file
    log_file = fopen("scheduler.log", "a");
    fprintf(log_file, "At time %d process %d stopped arr %d total %d remain %d wait %d\n",
            getClk(),
            curProc->id,
            curProc->arrivalTime,
            curProc->runningTime,
            curProc->remainingTime,
            curProc->waitingTime);
    fclose(log_file);
}
void write_in_logFile_resume()
{
    log_file = fopen("scheduler.log", "a");
    fprintf(log_file, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",
            getClk(),
            curProc->id,
            curProc->arrivalTime,
            curProc->runningTime,
            curProc->remainingTime,
            curProc->waitingTime);
    fclose(log_file);
}