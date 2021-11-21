#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

pthread_cond_t cond;
pthread_mutex_t mutex;

struct Node
{
    int number;
    struct Node* next;
}Node;
struct Node* head=NULL;

void* producer(void* arg)
{
    while(1)
    {


        pthread_mutex_lock(&mutex);
        struct Node* newNode=(struct Node*)malloc(sizeof(struct Node));
        newNode->number=rand()%1000;
        newNode->next=head;
        head=newNode;
        printf("生产者,id:%ld,number:  %d \n",pthread_self(),newNode->number);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
        sleep(rand()%3);
    }
    return NULL;
}

void* consumer (void* arg)
{
    while(1)
    {
        while(head==NULL)
        {
            pthread_cond_wait(&cond,&mutex);    
        }
        pthread_mutex_lock(&mutex);
       struct Node* node=head;
       printf("消费者，id:   %ld,number:   %d\n",pthread_self(),node->number);
       head=head->next;
       free(node);
       pthread_mutex_unlock(&mutex);
        sleep(rand()%3);
    }
    
    return NULL;
}
int main()
{
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);

    pthread_t c[5],p[8];
    for (int i=0;i<8;++i)
    {
        pthread_create(&p[i],NULL,producer,NULL);
    }
    for (int i=0;i<5;++i)
    {
        pthread_create(&c[i],NULL,consumer,NULL);
    }
    for (int i=0;i<8;++i)
    {
        pthread_join(p[i],NULL);
    }
    for (int i=0;i<5;++i)
    {
        pthread_join(c[i],NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}


