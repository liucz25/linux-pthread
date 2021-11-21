#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

#define MAX 50

int number;

pthread_rwlock rwlock;

void *read_func(void *arg)
{
    for (int i = 0; i < MAX; ++i)
    {
        pthread_rwlock_rdlock(&rwlock);
        printf("Thread READ R[%d],id =%lu,time=%d,numer=%d\n", i, pthread_self(), i, number);
        usleep(rand() % 5);
        pthread_rwlock_unlock(&rwlock);
    }
    return NULL;
}

void *write_func(void *arg)
{
    for (int i = 0; i < MAX; ++i)
    {
        pthread_rwlock_wrlock(&rwlock);
        int cur = number;
        cur++;
        printf("Thread wRITE W[%d],id =%lu,time=%d,numer=%d\n", i, pthread_self(), i, number);
        usleep(rand() % 5);
        number = cur;
        pthread_rwlock_unlock(&rwlock);
    }

    return NULL;
}

int main()
{
    pthread_t r[5], w[3];
    pthread_rwlock_init(&rwlock, NULL);

    for (int i = 0; i < 5; i++)
    {
        pthread_create(&r[i], NULL, read_func, NULL);
    }
    for (int i = 0; i < 3; i++)
    {
        pthread_create(&w[i], NULL, write_func, NULL);
    }

    for (int i = 0; i < 5; i++)
    {
        pthread_join(r[i], NULL);
    }
    for (int i = 0; i < 3; i++)
    {
        pthread_join(w[i], NULL);
    }

    pthread_rwlock_destory(&rwlock);

    return 0;
}
