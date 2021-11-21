#include <stdio.h>
#include "volatile.c"
extern volatile unsigned int timeOut;
int main()
{
    while(timeOut!=1);
    printf("Hello world\n");
    return 0;
}

