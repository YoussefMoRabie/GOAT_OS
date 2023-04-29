#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int TempClk;
bool cont = false;
void cont_handler(int);
int main(int argc, char *argv[])
{
    // TODO it needs to get the remaining time from somewhere
    signal(SIGCONT, cont_handler);
    initClk();
    // the runtime is passed from the scheduler as an argument
    remainingtime = atoi(argv[1]);

    TempClk = getClk();
    while (remainingtime > 0)
    {
        if (TempClk != getClk() || cont)
        {
            TempClk = getClk();
            if (cont)
            {
                cont = false;
            }
            else
                remainingtime--;
            printf("proc %d remain: %d\n", getpid(), remainingtime);
        }
    }
    kill(getppid(), SIGUSR1);
    destroyClk(false);
    return 0;
}
void cont_handler(int signum)
{
    TempClk = getClk();
    cont = true;
}
