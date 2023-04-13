#include "headers.h"

void clearResources(int);
int msgBuf;
PROC_BUFF newProc;

int main(int argc, char *argv[])
{
    // to be replaced with queue
    Queue process_queue;

    signal(SIGINT, clearResources);
     msgBuf=init_buff();
    newProc.m_type=1;

    // TODO Initialization
    // 1. Read the input files.
    FILE *ptr = fopen("processes.txt", "r");
    if (ptr == NULL)
    {
        printf("no such file.");
        return 0;
    }

    char p_id[16], p_arr[16], p_run[16], p_pri[16];
    while (fscanf(ptr, "%s %s %s %s", p_id, p_arr, p_run, p_pri) == 4)
    {
        // ignore comments
        if (p_id[0] == '#')
            continue;
        printf("%d %d %d %d\n", atoi(p_id), atoi(p_arr), atoi(p_run), atoi(p_pri));
        Process *p = (Process *)malloc(sizeof(Process));
        p->arrivalTime = atoi(p_arr);
        p->id = atoi(p_id);
        p->priority = atoi(p_pri);
        p->runningTime = atoi(p_run);
        p->remainingTime=p->runningTime;
        enqueue(&process_queue, p);
    }

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    char algo_id;
    while (algo_id != '1' && algo_id != '2' && algo_id != '3')
    {
        printf("Enter the schedueling algorithm \n 1.SRTN   2.HPF   3.Round Robin\n");
        scanf("%c", &algo_id);
    }
    // 3. Initiate and create the scheduler and clock processes.
    int sch_pid = fork();
    if (sch_pid == 0){

            execlp("./scheduler.out", "./scheduler.out",&algo_id, NULL);
            }

    int clk_pid = fork();
    if (clk_pid == 0)
        execlp("./clk.out", "./clk.out", NULL);

    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // DONE AT THE BEGINNING

    // 6. Send the information to the scheduler at the appropriate time.
    int current_time = getClk();
    while (!isEmpty(&process_queue) || 1) // to be replaced with queue !empty
    {
        // if the current time = the arrival of a process send it to the queue.
        if (current_time != getClk())
        {
            current_time = getClk();
            printf("current time is %d\n", current_time);
        }
        while(!isEmpty(&process_queue) && getClk() == front(&process_queue)->arrivalTime)
        {
            Process* toSnd=dequeue(&process_queue);
            newProc.proc.id=toSnd->id;
            newProc.proc.arrivalTime=toSnd->arrivalTime;
            newProc.proc.p_state=toSnd->p_state;
            newProc.proc.lastPreempt=toSnd->lastPreempt;
            newProc.proc.priority=toSnd->priority;
            newProc.proc.remainingTime=toSnd->remainingTime;
            newProc.proc.runningTime=toSnd->runningTime;
            newProc.proc.waitingTime=toSnd->waitingTime;

            int snd=msgsnd(msgBuf,&newProc,sizeof(newProc.proc),!IPC_NOWAIT);
            if (snd==-1)
            {
                perror("Error in Send!\n");
                exit(-1);
            }
            kill(sch_pid,SIGUSR2);
            //dequeue and send the top process to the scheduler's ready queue 
        }
    }
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    msgctl(msgBuf, IPC_RMID, (struct msqid_ds *)0);
    kill(getpid(), 9);
}
