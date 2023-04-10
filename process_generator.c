#include "headers.h"

void clearResources(int);

struct process
{
    int id;
    int arrival;
    int runtime;
    int priority;
};

struct p_q
{
    int count;
    struct process arr[100];
};

int main(int argc, char *argv[])
{
    //to be replaced with queue
    struct p_q queue;
    queue.count = 0;

    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    FILE *ptr = fopen("processes.txt", "r");
    if (ptr == NULL)
    {
        printf("no such file.");
        return 0;
    }


    char p_id[16],p_arr[16],p_run[16],p_pri[16];
    while (fscanf(ptr, "%s %s %s %s",p_id,p_arr,p_run,p_pri) == 4)
    {
        //ignore comments
        if(p_id[0] == '#')
            continue; 
        printf("%d %d %d %d\n",atoi(p_id),atoi(p_arr),atoi(p_run),atoi(p_pri));
        struct process p;
        p.arrival = atoi(p_arr);
        p.id = atoi(p_id);
        p.priority = atoi(p_pri);
        p.runtime = atoi(p_run);
        queue.arr[0] = p;
        queue.count++;
    }
    printf("the process count is %d\n",queue.count);


    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int algo_id = 0;
    while (algo_id > 3 || algo_id < 1)
    {
        printf("Enter the schedueling algorithm \n 1.SRTN   2.HPF   3.Round Robin\n");
        scanf("%d", &algo_id);
    }
    // 3. Initiate and create the scheduler and clock processes.

    //int sch_pid = fork();
    //if(sch_pid == 0)
    //    execlp("./sch","./sch",NULL); //SEND THE ALGO ID LATER

    int clk_pid = fork();
    if(clk_pid == 0)
        execlp("./c","./c",NULL);
    
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    //DONE AT THE BEGINNING

    // 6. Send the information to the scheduler at the appropriate time.
    int current_time;
    while (1) // to be replaced with queue !empty
    {
        //if the current time = the arrival of a process send it to the queue.
        current_time = getClk();
        sleep(1);
        printf("current time is %d\n", current_time);       
    }
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    kill(getpid(), 9);
}
