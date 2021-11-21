# linux 多线程

## 线程创建 

```c
pthread_t pthread_self(void);//返回当前线程的线程ID



#include <pthread.h>   ////库的全名   libpthread.so

int pthread_create(pthread_t *thread,const pthread_attr_t *attr,void *(*start_routime)(void *),void *arg)
    
#thread 传出参数，线程创建成功，会将线程ID写入这个指针指向的内存中
#attr 线程的属性 默认写NULL
#函数指针，线程要执行的任务 **void* 类型**
#*arg 线程要执行函数的参数

```
函数指针，线程要执行的任务 **void* 类型**

例子

```c
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>  //库的全名   libpthread.so

void* callback(void* arg)
{
    for (int i=0;i<5;++i){
        printf("子线程：i=%d\n",i);
    }
    printf("子线程id：%ld\n",pthread_self());
    return NULL;
 }

int main()
{
    pthread_t tid1;
    pthread_create(&tid1,NULL,callback,NULL);
    for (int i=0;i<5;++i){
        printf("主线程：i=%d\n",i);
    }
    printf("主线程id：%ld\n",pthread_self());
    sleep(1);
    return 0;
}

```

sudo apt-get install build-essential //安装编译的头文件 

 gcc pthread_create_test.c -lpthread //用线程库  **-lpthread**

## 线程退出

```c
#include <prhtead.h>
void pthread_exit(void *retval);
```
例子
```c
把上边的例子的sleep（） 换成pthread_exit(NULL);
```

## 线程回收

子线程退出需要主线程回收  回收内核资源

```c
#include <pthread.h>
int pthread_jion(pthread_t thread,void **retval);
// thread 要回收的子线程的ID
// retval 二级指针，指向一级指针地址，是一个传出参数，这个地址存储了pthread_exit()传递出的数据，如果不需要，可以指定为NULL
//返回值 成功0 回收失败返回错误号
//阻塞函数  每次回收一个
```
例子
```c
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

    void* ptr;
    pthread_join(tid1,&ptr);
    struct Test* pt =(struct Test*)ptr;
    printf("num:%d,age=%d\n",pt->num,pt->age);
    
    return 0;
}


```
问题 内存区域被释放  需要是堆内存或全局变量
全局变量  把 struct Test t ； 放大函数外部
堆内存 main中声明 传入到函数中，在函数中赋值，在main中使用    //感觉特别有用 应该内实现解耦合的一些操作    重要 
堆内存问题  线程安全 多个线程同时改该变量 无法同步 最终结构出错   解决方法 加锁

## 线程分离
原因： join 阻塞   回收时暂停

```c
#include <pthread.h>
int pthread_detach(pthread_t thread);
// thread 要分离的子线程的ID
```
例子
```c
main()函数中添加如下：
pthread_detach(tid);
pthread_exit(NULL);//主线程自己退出 不耽误子线程的空间
```
## 线程取消
主线程中  取消 子线程不一定死 需要满足一定的条件  达到系统调用处   不一定是程序直接调用系统调用

```c
#include <pthread.h>
int pthread_cancel(pthread_t thread);
// thread 要取消的子线程的ID
```

## 线程比较

```c
#include <pthread.h>
int pthread_equal（pthread_t t1，pthread_t t2);
```
不用这样做，pthread 是无符号整型变量，不用这样麻烦，这样时代发展，留着备用的

# 线程同步 锁
## 互斥锁
```c
//声明
pthread_mutex_t mutex;

//初始化

int pthread_mutex_init(pthread_mutex_t *restrict mutex,
					const pthread_mutex_t *restict attr);
//释放互斥锁资源
int pthread_mutex_destroy(pthread_mutex_t *mutex);


//mutex 互斥锁变量的地址
//attr 默认属性 NULL即可
//restrict 是限制 不允许定义指针，通过指针修改值，起到保护程序的作用


//使用互斥锁
int pthread_mutex_lock(pthread_mutex_t *mutex);

//尝试加锁，非阻塞
int pthread_mutex_trylock(pthread_mutex_t *mutex);

//解锁
int pthread_mutex_unlock(pthread_mutex_t *mutex);


						
```

例子

```c
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
        pthread_mutex_trylock(&mutex);
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
        pthread_mutex_trylock(&mutex);
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

    return 0;
}


```

## 死锁

### 忘记解锁 

​	明显的 		return前的

### 重复加锁 

​    条件判断等  函数调用等

### 多个共享资源 

资源内嵌套      多少个共享资源多少个锁

trylock 可解决一部分死锁问题

加锁前解锁 解别的锁 可以解决部分

项目中可以引入一些检测死锁模块

## 读写锁

一把锁，锁定读 锁定写
读锁是共享的  写锁是独占的   写锁比读锁的优先级高       读多写少 效率比互斥锁好

```c
//声明
pthread_rwlock_t rwlock;
//初始化
int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,
					const pthread_rwlockattr_t *restrict attr);
//释放
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);

//上读锁
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
//试上读锁
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);				
//上写锁
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
//试上写锁
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
//解锁 解读 解写 都用这个
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);

```

例子
```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

#define MAX 50

int number=1;

pthread_rwlock_t rwlock;

void *read_func(void *arg)
{
    for (int i = 0; i < MAX; ++i)
    {
        pthread_rwlock_rdlock(&rwlock);
        printf("Thread READ R[%d],id =%lu,time=%d,number=%d\n", i, pthread_self(), i, number);
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
        printf("Thread wRITE W[],id =%lu,time=%d,number=%d\n",  pthread_self(), i, number);
        usleep(5);
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

    pthread_rwlock_destroy(&rwlock);

    return 0;
}

```

## 条件变量


不能进程同步的问题，需要锁，例如互斥锁。
应用于 生产～消费模型
条件变量，处理任务队列，满，阻塞生产者，空阻塞消费者

```c
//声明
pthread_cond_t cond;
保存了线程id

//初始化
int pthread_cond_init(pthread_cont_t *cond)

//销毁
int pthread_cond_destroy(pthread_cont_t *cond)

//使用
//1.wait
int pthread_cond_wait(pthread_cond_t *restrict cond,pthread_mutex_t *restrict mutex);
参数，条件变量，互斥锁
    阻塞线程，线程同步

//2.timewait   时间
int pthread_cond_wait(pthread_cond_t *restrict cond,pthread_mutex_t *restrict mutex,           
const struct timespec *restrict abstime);
//time 从1970.01.01到某个时间的秒数

//附例：
    time_t mytime=time(NULL);
	struct timwspec tmsp;
	tmsp.tv_nsec=0;
	tmsp.tv_sec=time(NULL)+100;//阻塞100秒

//唤醒
int pthread_cond_signal(pthread_cond_t *cond);//唤醒一个
int pthread_cond_broascast(pthread_cond_t *cond);//唤醒所有


```

例子

```c
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
        pthread_mutex_lock(&mutex);
        while(head==NULL)
        {
            pthread_cond_wait(&cond,&mutex);   //此处不会死锁，cond 在阻塞时，如果线程已经加锁，会解开互斥锁，以防止死锁。pthread_cond_wait函数自动解锁开锁适应
        }

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



```
## 信号量 
库存 仓库   停车场的例子 
信号量（信号灯）与互斥锁和条件变量的主要不同在于” 灯” 的概念，灯亮则意味着资源可用，灯灭则意味着不可用。信号量主要阻塞线程，不能完全保证线程安全，如果要保证线程安全，需要信号量和互斥锁一起使用

![停车](D:\code\linux-phread\2021-11-12 10_39_24-Window.png)

注意判定条件


```c
#include <semaphore.h>
sem_t sem;

//初始化
int sem_init(sem_t *sem,int pshared,unsigned int value);
//销毁
int sem_destory(sem_t *sem);

//阻塞
int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
int sem_timewait(sem_t *sem,const struct timespec *abs_timeout);
//timespec 同上；
    
// 调用该函数给sem中的资源数+1
int sem_post(sem_t *sem);


// 查看信号量 sem 中的整形数的当前值, 这个值会被写入到sval指针对应的内存中
// sval是一个传出参数
int sem_getvalue(sem_t *sem, int *sval);

```

```c

模板

// 生产者线程 
sem_t psem;
// 消费者线程
sem_t csem;

// 信号量初始化
sem_init(&psem, 0, 5);    // 5个生产者可以同时生产
sem_init(&csem, 0, 0);    // 消费者线程没有资源, 因此不能消费

// 生产者线程
// 在生产之前, 从信号量中取出一个资源
sem_wait(&psem);	
// 生产者商品代码, 有商品了, 放到任务队列
......	 
......
......
// 通知消费者消费，给消费者信号量添加资源，让消费者解除阻塞
sem_post(&csem);
	
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

// 消费者线程
// 消费者需要等待生产, 默认启动之后应该阻塞
sem_wait(&csem);
// 开始消费
......
......
......
// 消费完成, 通过生产者生产，给生产者信号量添加资源
sem_post(&psem);

```



总资源数为 1

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

// 链表的节点
struct Node
{
    int number;
    struct Node* next;
};

// 生产者线程信号量
sem_t psem;
// 消费者线程信号量
sem_t csem;

// 互斥锁变量
pthread_mutex_t mutex;
// 指向头结点的指针
struct Node * head = NULL;

// 生产者的回调函数
void* producer(void* arg)
{
    // 一直生产
    while(1)
    {
        // 生产者拿一个信号灯
        sem_wait(&psem);
        // 创建一个链表的新节点
        struct Node* pnew = (struct Node*)malloc(sizeof(struct Node));
        // 节点初始化
        pnew->number = rand() % 1000;
        // 节点的连接, 添加到链表的头部, 新节点就新的头结点
        pnew->next = head;
        // head指针前移
        head = pnew;
        printf("+++producer, number = %d, tid = %ld\n", pnew->number, pthread_self());

        // 通知消费者消费, 给消费者加信号灯
        sem_post(&csem);
        

        // 生产慢一点
        sleep(rand() % 3);
    }
    return NULL;
}

// 消费者的回调函数
void* consumer(void* arg)
{
    while(1)
    {
        sem_wait(&csem);
        // 取出链表的头结点, 将其删除
        struct Node* pnode = head;
        printf("--consumer: number: %d, tid = %ld\n", pnode->number, pthread_self());
        head  = pnode->next;
        free(pnode);
        // 通知生产者生成, 给生产者加信号灯
        sem_post(&psem);

        sleep(rand() % 3);
    }
    return NULL;
}

int main()
{
    // 初始化信号量
    // 生产者和消费者拥有的信号灯的总和为1
    sem_init(&psem, 0, 1);  // 生成者线程一共有1个信号灯
    sem_init(&csem, 0, 0);  // 消费者线程一共有0个信号灯

    // 创建5个生产者, 5个消费者
    pthread_t ptid[5];
    pthread_t ctid[5];
    for(int i=0; i<5; ++i)
    {
        pthread_create(&ptid[i], NULL, producer, NULL);
    }

    for(int i=0; i<5; ++i)
    {
        pthread_create(&ctid[i], NULL, consumer, NULL);
    }

    // 释放资源
    for(int i=0; i<5; ++i)
    {
        pthread_join(ptid[i], NULL);
    }

    for(int i=0; i<5; ++i)
    {
        pthread_join(ctid[i], NULL);
    }

    sem_destroy(&psem);
    sem_destroy(&csem);

    return 0;
}


作者: 苏丙榅
链接: https://subingwen.cn/linux/thread-sync/#6-2-%E7%94%9F%E4%BA%A7%E8%80%85%E5%92%8C%E6%B6%88%E8%B4%B9%E8%80%85
来源: 爱编程的大丙
著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。
```

总是大于1

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

// 链表的节点
struct Node
{
    int number;
    struct Node* next;
};

// 生产者线程信号量
sem_t psem;
// 消费者线程信号量
sem_t csem;

// 互斥锁变量
pthread_mutex_t mutex;
// 指向头结点的指针
struct Node * head = NULL;

// 生产者的回调函数
void* producer(void* arg)
{
    // 一直生产
    while(1)
    {
        // 生产者拿一个信号灯
        sem_wait(&psem);
        // 加锁, 这句代码放到 sem_wait()上边, 有可能会造成死锁
        pthread_mutex_lock(&mutex);// ********************
        // 创建一个链表的新节点
        struct Node* pnew = (struct Node*)malloc(sizeof(struct Node));
        // 节点初始化
        pnew->number = rand() % 1000;
        // 节点的连接, 添加到链表的头部, 新节点就新的头结点
        pnew->next = head;
        // head指针前移
        head = pnew;
        printf("+++producer, number = %d, tid = %ld\n", pnew->number, pthread_self());
        pthread_mutex_unlock(&mutex);// ********************

        // 通知消费者消费
        sem_post(&csem);
        
        // 生产慢一点
        sleep(rand() % 3);
    }
    return NULL;
}

// 消费者的回调函数
void* consumer(void* arg)
{
    while(1)
    {
        sem_wait(&csem);
        pthread_mutex_lock(&mutex);// ********************
        struct Node* pnode = head;
        printf("--consumer: number: %d, tid = %ld\n", pnode->number, pthread_self());
        head  = pnode->next;
        // 取出链表的头结点, 将其删除
        free(pnode);
        pthread_mutex_unlock(&mutex);// ********************
        // 通知生产者生成, 给生产者加信号灯
        sem_post(&psem);

        sleep(rand() % 3);
    }
    return NULL;
}

int main()
{
    // 初始化信号量
    sem_init(&psem, 0, 5);  // 生成者线程一共有5个信号灯
    sem_init(&csem, 0, 0);  // 消费者线程一共有0个信号灯
    // 初始化互斥锁
    pthread_mutex_init(&mutex, NULL);

    // 创建5个生产者, 5个消费者
    pthread_t ptid[5];
    pthread_t ctid[5];
    for(int i=0; i<5; ++i)
    {
        pthread_create(&ptid[i], NULL, producer, NULL);
    }

    for(int i=0; i<5; ++i)
    {
        pthread_create(&ctid[i], NULL, consumer, NULL);
    }

    // 释放资源
    for(int i=0; i<5; ++i)
    {
        pthread_join(ptid[i], NULL);
    }

    for(int i=0; i<5; ++i)
    {
        pthread_join(ctid[i], NULL);
    }

    sem_destroy(&psem);
    sem_destroy(&csem);
    pthread_mutex_destroy(&mutex);

    return 0;
}
```

在编写上述代码的时候还有一个需要注意是事项，不管是消费者线程的处理函数还是生产者线程的处理函数内部有这么两行代码：

```c
// 消费者
sem_wait(&csem);
pthread_mutex_lock(&mutex);

// 生产者
sem_wait(&csem);
pthread_mutex_lock(&mutex);
这两行代码的调用顺序是不能颠倒的，如果颠倒过来就有可能会造成死锁，
```













