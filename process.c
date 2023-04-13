#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int TempClk;
void cont_handler(int);
int main(int argc, char * argv[])
{   
    signal(SIGCONT,cont_handler);
    initClk();
    remainingtime=atoi(argv[1]);

    //TODO it needs to get the remaining time from somewhere
    //remainingtime = runtime "passed from args";
    TempClk=getClk();
    while (remainingtime > 0)
    {
        if (TempClk!=getClk())
        {
            printf("remaining: %d\n",remainingtime);
            TempClk=getClk();
            remainingtime--;

        }
        // remainingtime = ??;
    }
    printf("remaining: %d\n",remainingtime);
    kill(getppid(),SIGUSR1);
    destroyClk(false);
    return 0;
}
void cont_handler(int signum){
    TempClk=getClk();
    
}
