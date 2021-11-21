#ifndef _TTHREADPOOL_H
#define _THREADPOOL_H
//线程池结构体
typedef struct ThreadPool ThreadPool;
//创建线程池并初始化

ThreadPool *threadPoolCreate(int min, int max, int queueSize);

//销毁线程池
//给线程池添加任务
//获取线程池  工 作  的线程的个数
//获取线程池  活 着  的线程的个数

//
void *worker(void *arg);
void *manager(void *arg);

#endif //_THREADPOOL_H