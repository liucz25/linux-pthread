#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>  //库的全名   libpthread.so

struct Test
{
    int num;
    int age;
};

void* callback(void* arg)
{
    for (int i=0;i<5;++i)
    {
        printf("子线程：i=%d\n",i);
    }
    printf("子线程id：%ld\n",pthread_self());

    struct Test t;
    t.num=9;
    t.age=33;

    pthread_exit(&t);

    return NULL;
}

int main()
{
    pthread_t tid1;
    pthread_create(&tid1,NULL,callback,NULL);
    for (int i=0;i<5;++i)
    {
        printf("主线程：i=%d\n",i);
    }
    printf("主线程id：%ld\n",pthread_self());

    pthread_detach(tid1);
    pthread_exit(NULL);
    return 0;
}
