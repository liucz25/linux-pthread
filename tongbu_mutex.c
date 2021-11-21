#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

#define MAX 50

int number;

pthread_mutex_t mutex;
void* funcA_num(void* arg)
{
    for (int i=0;i<MAX;++i)
    {
        pthread_mutex_lock(&mutex);
        int cur=number;
        cur++;
        usleep(7);
        number=cur;
        printf("Thread A ,id =%lu,time=%d,numer=%d\n",pthread_self(),i,number);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}


void* funcB_num(void* arg)
{
        //pthread_mutex_lock(&mutex);
    for (int i=0;i<MAX;++i)
    {
        pthread_mutex_lock(&mutex);
        int cur=number;
        cur++;
        number=cur;
        printf("Thread B ,id =%lu,time=%d,numer=%d\n",pthread_self(),i,number);
        //usleep(5);
        pthread_mutex_unlock(&mutex);
        usleep(4);
    }
        //pthread_mutex_unlock(&mutex);
    return NULL;
}




int main()
{
    pthread_t p1,p2;
    pthread_mutex_init(&mutex,NULL);
    pthread_create(&p1,NULL,funcA_num,NULL);
    pthread_create(&p2,NULL,funcB_num,NULL);
    //pthread_detach(p1);
    //pthread_detach(p2);
    //pthread_exit(NULL);
    pthread_join(p1,NULL);
    pthread_join(p2,NULL);
    pthread_mutex_destroy(&mutex);




    //printf("Hello world\n");
    return 0;
}

