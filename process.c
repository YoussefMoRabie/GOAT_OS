#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int TempClk;
bool cont=false;
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
        if (TempClk!=getClk()|| cont)
        {
            TempClk=getClk();
            if(cont){
                cont=false;
            }else
            remainingtime--;
            printf("Proc: remaining: %d\n",remainingtime);
        }
        // remainingtime = ??;
    }
    kill(getppid(),SIGUSR1);
    destroyClk(false);
    return 0;
}
void cont_handler(int signum){
    TempClk=getClk();
    cont=true;
    
}
