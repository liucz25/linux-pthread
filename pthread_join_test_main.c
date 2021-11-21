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

    //struct Test t;
    
    struct Test* t=(struct Test*)arg;
    t->num=9;
    t->age=33;

    pthread_exit(&t);

    return NULL;
}

int main()
{
    struct Test t;

    pthread_t tid1;
    pthread_create(&tid1,NULL,callback,&t);//添加参数地址

    for (int i=0;i<5;++i)
    {
        printf("主线程：i=%d\n",i);
    }
    printf("主线程id：%ld\n",pthread_self());

    void* ptr;
    pthread_join(tid1,&ptr);
    //struct Test* pt =(struct Test*)ptr;
    printf("num:%d,age=%d\n",t.num,t.age);
    
    return 0;
}
