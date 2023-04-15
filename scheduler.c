#include "headers.h"

void Termination_SIG(int signnum);
void newProc_arrive(int signum);
void algo_start();
void output_performance();
Process *curProc;
Queue *RQ;
int TempCLK;
PROC_BUFF newProc;
int msgBuf;
int algoId;
void (*algo_func)();
bool switched;
bool new_arrive = false;
bool cur_terminated = false;

// variables for the output files
FILE *log_file;
FILE *perf;
int process_count;
int tot_Wait = 0;
int tot_runtime = 0;
float *WTA_ARR; // array of WTA, used in scheduler.log math
int WTA_INDEX = 0;

void SRTN();
void HPF();

int main(int argc, char *argv[])
{
    initClk();
    init_sim_state();

    signal(SIGUSR1, Termination_SIG);
    signal(SIGUSR2, newProc_arrive);

    RQ = Queue_init();
    algoId = atoi(argv[1]);

    msgBuf = init_buff();
    curProc == NULL;

    process_count = atoi(argv[2]);
    WTA_ARR = malloc(process_count * sizeof(float));

    perf = fopen("scheduler.log","w");
    fprintf(perf,"#At time x process y state arr w total z remain y wait k\n");
    fclose(perf);

    printf("scheduler is working with algorithm id: %s\n", argv[1]);
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
    else
    { // else you update the current process params

        /* TO BE REMOVED LATER
        if (curProc->remainingTime==0)
              return;
        */

        if (!new_arrive) // it doesn't update if a new process arrived cuz it is already updated
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
        // for testing
        printf("At time %d process %d stopped arr %d total %d remain %d wait %d\n",
               getClk(),
               curProc->id,
               curProc->arrivalTime,
               curProc->runningTime,
               curProc->remainingTime,
               curProc->waitingTime);

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
        // for testing
        printf("At time %d process %d started arr %d total %d remain %d wait %d\n",
               getClk(),
               curProc->id,
               curProc->arrivalTime,
               curProc->runningTime,
               curProc->remainingTime,
               curProc->waitingTime);

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
    else
    {
        if (switched)
        {
            // resume the process if there is context switch
            switched = false;
            curProc->waitingTime += getClk() - curProc->lastPreempt;
            // for testing
            printf("At time %d process %d resumed arr %d total %d remain %d wait %d\n",
                   getClk(),
                   curProc->id,
                   curProc->arrivalTime,
                   curProc->runningTime,
                   curProc->remainingTime,
                   curProc->waitingTime);

            // output to the scheduler.log file
            log_file = fopen("scheduler.log", "a");
            fprintf(log_file, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",
                    getClk(),
                    curProc->id,
                    curProc->arrivalTime,
                    curProc->runningTime,
                    curProc->remainingTime,
                    curProc->waitingTime);
            fclose(log_file);
            kill(curProc->pid, SIGCONT);
        }
    }
}

void HPF()
{
}

void Termination_SIG(int signnum)
{
    int TA = getClk() - curProc->arrivalTime;
    float WeTA = (float)TA / curProc->runningTime;

    tot_Wait += curProc->waitingTime;
    tot_runtime += curProc->runningTime;
    WTA_ARR[WTA_INDEX] = WeTA;
    WTA_INDEX++;
    // for testing
    printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",
           getClk(),
           curProc->id,
           curProc->arrivalTime,
           curProc->runningTime,
           curProc->remainingTime,
           curProc->waitingTime,
           TA,
           WeTA);

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
}

void newProc_arrive(int signnum)
{
    new_arrive = true;
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

        // to be modified based on algoId
        if (algoId != '1')
        {
            InsertWithPriority(RQ, p, p->remainingTime);
        }
        else if (algoId != '2')
        {
            InsertWithPriority(RQ, p, p->priority);
        }
        else
        {
            // insertion for RR
        }

    } while (i);
}

// outputs the scheduler.perf at the end of the simulation
void output_performance()
{
    int curr_time = getClk();
    float CPU_utilization = ( tot_runtime / (float)curr_time) * 100;
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

    printf("proc_count: %d curr_time: %d tot_runtime: %d tot_wait: %d\n",process_count,curr_time,tot_runtime,tot_Wait);
    perf = fopen("scheduler.perf", "w");
    fprintf(perf, "CPU utilization = %f\n", CPU_utilization);
    fprintf(perf, "Avg WTA = %f\n", WTA_AVG);
    fprintf(perf, "Avg Waiting = %f\n", avg_wait);
    fprintf(perf, "Std WTA = %f\n", WTA_STD);
    fclose(perf);
}