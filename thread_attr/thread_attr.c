/** thread_attr.c */

/**
 * pthread_create()函数的第二个参数（pthread_attr_t *attr）表示线程的属性,将该值设为 NULL，
 * 也就是采用默认属性，线程的多项属性都是可以更改的。这些属性主要包括绑定属性、分离属性、堆栈地址、
 * 堆栈大小以及优先级。其中系统默认的属性为非绑定、非分离、缺省 1M 的堆栈以及与父进程同样级别的优先级。
 * 
 *   绑定属性。
 * Linux 中采用“一对一”的线程机制，也就是一个用户线程对应一个内核线程。绑定属性就是指一个用户线程固定地
 * 分配给一个内核线程，因为 CPU 时间片的调度是面向内核线程（也就是轻量级进程）的，因此具有绑定属性的线程
 * 可以保证在需要的时候总有一个内核线程与之对应。而与之对应的非绑定属性就是指用户线程和内核线程的关系不是
 * 始终固定的，而是由系统来控制分配的。
 * 
 *   分离属性。
 * 分离属性是用来决定一个线程以什么样的方式来终止自己。在非分离情况下，当一个线程结束时，它所占用的系统资源
 * 并没有被释放，也就是没有真正的终止。只有当 pthread_join()函数返回时，创建的线程才能释放自己占用的系统资源。
 * 而在分离属性情况下，一个线程结束时立即释放它所占有的系统资源。这里要注意的一点是， 如果设置一个线程的分离属性，
 * 而这个线程运行又非常快， 那么它很可能在 pthread_create()函数返回之前就终止了，它终止以后就可能将线程号
 * 和系统资源移交给其他的线程使用，这时调用pthread_create()的线程就得到了错误的线程号。
 * 
 * 这些属性的设置都是通过特定的函数来完成的，通常首先调用 pthread_attr_init()函数进行初始化，之后再调用相应
 * 的属性设置函数，最后调用 pthread_attr_destroy()函数对分配的属性结构指针进行清理和回收。设置绑定属性的函
 * 数为 pthread_attr_setscope()，设置线程分离属性的函数为 pthread_attr_setdetachstate()，设置线程优先
 * 级的相关函数为 pthread_attr_getschedparam()（获取线程优先级）和 pthread_attr_setschedparam()（设置线程优先级）。
 * 在设置完这些属性后，就可以调用 pthread_create()函数来创建线程了。
 * 
 * 函数原型 int pthread_attr_init(pthread_attr_t *attr)
 * 函数传入值 attr：线程属性结构指针
 * 函数返回值
 *  成功： 0
 *  出错：返回错误码
 * 
 * 函数原型 int pthread_attr_setscope(pthread_attr_t *attr, int scope)
 * 函数传入值 attr：线程属性结构指针
 * cope
 *  PTHREAD_SCOPE_SYSTEM：绑定
 *  PTHREAD_SCOPE_PROCESS：非绑定
 * 函数返回值
 *  成功： 0
 *  出错： -1
 * 
 * 函数原型 int pthread_attr_setscope(pthread_attr_t *attr, int detachstate)
 * 函数传入值
 *  attr：线程属性
 *  detachstate:
 *   PTHREAD_CREATE_DETACHED：分离
 *   PTHREAD _CREATE_JOINABLE：非分离
 * 函数返回值
 *  成功： 0
 *  出错：返回错误码
 * 
 * 函数原型 int pthread_attr_getschedparam (pthread_attr_t *attr, struct sched_param *param)
 * 函数传入值
 *  attr：线程属性结构指针
 *  param：线程优先级
 * 函数返回值
 *  成功： 0
 *  出错：返回错误码
 * 
 * 
 * 函数原型 int pthread_attr_setschedparam (pthread_attr_t *attr, struct sched_param *param)
 * 函数传入值
 *  attr：线程属性结构指针
 *  param：线程优先级
 * 函数返回值
 *  成功： 0
 *  出错：返回错误码
 * 
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define REPEAT_NUMBER 3 /* 线程中的小任务数 */
#define DELAY_TIME_LEVELS 10.0 /* 小任务之间的最大时间间隔 */
int finish_flag = 0;

void *thrd_func(void *arg)
{
    int delay_time = 0;
    int count = 0;
    printf("Thread is starting\n");
    for (count = 0; count < REPEAT_NUMBER; count++)
    {
        delay_time = (int)(rand() * DELAY_TIME_LEVELS/(RAND_MAX)) + 1;
        sleep(delay_time);
        printf("\tThread : job %d delay = %d\n", count, delay_time);
    }

    printf("Thread finished\n");
    finish_flag = 1;
    pthread_exit(NULL);
}


int main(void)
{
    pthread_t thread;
    pthread_attr_t attr;
    int no = 0, res;
    void * thrd_ret;

    srand(time(NULL));

    /* 初始化线程属性对象 */
    res = pthread_attr_init(&attr);
    if (res != 0)
    {
        printf("Create attribute failed\n");
        exit(res);
    }

    /* 设置线程绑定属性 */
    res = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    /* 设置线程分离属性 */
    res = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (res != 0)
    {
        printf("Setting attribute failed\n");
        exit(res);
    }

    res = pthread_create(&thread, &attr, thrd_func, NULL);
    if (res != 0)
    {
        printf("Create thread failed\n");
        exit(res);
    }

    /* 释放线程属性对象 */
    pthread_attr_destroy(&attr);

    printf("Create tread success\n");
    while(!finish_flag)
    {
        printf("Waiting for thread to finish...\n");
        sleep(1);
    }

    return 0;
}


