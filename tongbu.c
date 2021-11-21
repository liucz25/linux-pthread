#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

#define MAX 50

int number;

void* funcA_num(void* arg)
{
    for (int i=0;i<MAX;++i)
    {
        int cur=number;
        cur++;
        usleep(10);
        number=cur;
        printf("Thread A ,id =%lu,time=%d,numer=%d\n",pthread_self(),i,number);
    }
    return NULL;
}


void* funcB_num(void* arg)
{
    for (int i=0;i<MAX;++i)
    {
        int cur=number;
        cur--;
        usleep(5);
        number=cur;
        printf("Thread B ,id =%lu,time=%d,numer=%d\n",pthread_self(),i,number);
    }
    return NULL;
}




int main()
{
    pthread_t p1,p2;
    pthread_create(&p1,NULL,funcA_num,NULL);
    pthread_create(&p2,NULL,funcB_num,NULL);
    pthread_join(p1,NULL);
    pthread_join(p2,NULL);

    //pthread_detach(&p1);
    //pthread_detach(&p2);
    //pthread_exit(NULL);
    //printf("counter=%d\n",number);





    //printf("Hello world\n");
    return 0;
}

