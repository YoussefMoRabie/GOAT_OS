#include "headers.h"


int main(int argc, char * argv[])
{
    initClk();
    
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
