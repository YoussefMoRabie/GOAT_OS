#include "headers.h"

typedef enum pState{
    Ready,
    Running,
}pState;

typedef struct PCB{
    int pid;
    pState p_state;
    int remaining_time;
    int running_time;
    int waiting_time;

} PCB;

typedef struct process_table{
    //to be continued
} process_table;

int main(int argc, char * argv[])
{
    initClk();
    Queue *RQ_addr= attach_RQ();
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    
    //remove this later
    printf("scheduler is working with algorithm id: %s", argv[1]);
    while(1)
    {
        sleep(100);
    }
    
    destroyClk(true);
}
