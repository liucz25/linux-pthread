
#include "threadpool.h"
#include <pthread.h>
//任务结构体
typedef struct Task
{
    void (*function)(void *arg);
    void *arg;

} Task;

//线程池结构体
struct ThreadPool
{
    //任务队列
    Task *taskQ;
    int queueCapacity; //容量
    int queueSize;     //当前任务数
    int queueFront;    //队头-》取数据
    int queueRear;     //队尾-》放数据

    pthread_t managerID; //管理者线程ID
    pthread_t threadIDs; //工作的线程ID
    int minNum;          //最小线程数
    int maxNum;          //最大
    int busyNum;         //忙
    int liveNum;         //存活
    int exitNum;         //要销毁

    pthread_mutex_t mutexpool; //锁定整个线程池
    pthread_mutex_t mutexBusy; //锁定busyNum变量

    int shutdown; //销毁1，不销毁0

    pthread_cond_t notFull;
    pthread_cond_t notEmpty;
};

ThreadPool *threadPoolCreate(int min, int max, int queueSize)
{
    ThreadPool *pool = (ThreadPool *)malloc(sizeof(ThreadPool));
    do //想要用break  防止释放资源麻烦   break->释放资源
    {
        if (pool == NULL)
        {
            printf("malloc threadpool fail...\n");
            break;
        }
        pool->threadIDs = (pthread_t)malloc(sizeof(pthread_t) * max); //分配空间
        if (pool->threadIDs == NULL)
        {
            printf("malloc threadIDs fail...\n");
            break;
        }
        memset(pool->threadIDs, 0, sizeof(pthread_t) * max); //初始化
        pool->minNum = min;
        pool->maxNum = max;
        pool->busyNum = 0;
        pool->liveNum = min;
        pool->exitNum = 0;
        if (pthread_mutex_init(&pool->mutexpool, NULL) != 0 ||
            pthread_mutex_init(&pool->mutexBusy, NULL) != 0 ||
            pthread_cond_init(&pool->notEmpty, NULL) != 0 ||
            pthread_cond_init(&pool->notFull, NULL) != 0)
        {
            printf("mutex or cond init fail...\n");
            break;
        }
        //任务队列
        pool->taskQ = (Task *)malloc(sizeof(Task) * queueSize);
        pool->queueCapacity = queueSize;
        pool->queueSize = 0;
        pool->queueFront = 0;
        pool->queueRear = 0;

        pool->shutdown = 0;

        //创建线程
        pthread_create(&pool->managerID, NULL, manager, pool);
        for (int i = 0; i < min; ++i)
        {
            pthread_create(&pool->threadIDs[i], NULL, worker, pool);
        }
        return pool;

    } while (0);

    //释放资源
    if (pool && pool->threadIDs)
        free(pool->threadIDs);
    if (pool && pool->taskQ)
        free(pool->taskQ);
    if (pool)
        free(pool);

    return NULL;
}

void *worker(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    while (1)
    {
        pthread_mutex_lock(&pool->mutexpool);
        //当前任务队列是否为空
        while (pool->queueSize == 0 && !pool->shutdown)
        {
            //阻塞工作线程
            pthread_cond_wait(&pool->notEmpty, &pool->mutexpool);
        }
        //判断线程池是否被关闭了
        if (pool->shutdown)
        {
            pthread_mutex_unlock(&pool->mutexpool);
            pthread_exit(NULL);
        }
        //从任务队列中取出一个任务
        Task task;
        task.function = pool->taskQ[pool->queueFront].function;
        task.arg = pool->taskQ[pool->queueFront].arg;
        //移动头结点
        pool->queueFront = (pool->queueFront + 1) % (pool->queueCapacity);
        pool->queueSize--;
        //解锁
        pthread_mutex_unlock(&pool->mutexpool);

        pthread_mutex_lock(&pool->mutexBusy);
        pool->busyNum++;
        pthread_mutex_unlock(&pool->mutexBusy);

        printf("thread %ld start working...\n");
        //执行
        task.function(task.arg);
        //(*task.function)(task.arg);
        free(task.arg);
        task.arg = NULL;
        printf("thread %ld end working...\n");

        pthread_mutex_lock(&pool->mutexBusy);
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexBusy);
    }

    return NULL;
}

void *manager(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    while (pool->shutdown)
    {
        //每隔3秒检测一次
        sleep(3);
        pthread_mutex_lock(&pool->mutexpool);
        int queueSize = pool->queueSize;
        int liveNum = pool->busyNum;

        pthread_mutex_unlock(&pool->mutexpool);

        pthread_mutex_lock(&pool->mutexBusy);

        pthread_mutex_unlock(&pool->mutexBusy);
    }
}