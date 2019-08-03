/** wait.c */

/**
 * wait()函数是用于使父进程（也就是调用 wait()的进程）阻塞，直到任何一个子进程结束或者该进程接到了一个
 * 指定的信号为止。如果该父进程没有子进程或者他的子进程已经结束，则 wait()就会立即返回。
 * waitpid()的作用和 wait()一样，但它并不一定要等待第一个终止的子进程，它还有若干选项，如可提供一个
 * 非阻塞版本的 wait()功能，也能支持作业控制。实际上 wait()函数只是 waitpid()函数的一个特例，在 Linux
 * 内部实现 wait()函数时直接调用的就是 waitpid()函数。
 */

/**
 * wait()函数用来帮助父进程获取其子进程的退出状态。当进程退出时，内核为每一个进程保存了一定量的退出状态信息，
 * 父进程可根据此退出信息来判断子进程的运行状况。如果父进程未调用 wait()函数，则子进程的退出信息将一直保存在内存中。
 * 由于进程终止的异步性，可能会出现子进程先终止或者父进程先终止的情况，从而出现两种特殊的进程：
 *   僵尸进程：如果子进程先终止，但其父进程没有为它调用 wait()函数，那么该子进程就会变为僵尸进程。
 *    僵尸进程在它的父进程为它调用 wait()函数之前将一直占有系统的内存资源。
 *   孤儿进程：如果父进程先终止，尚未终止的子进程将会变成孤儿进程。孤儿进程将直接被 init 进程收管，由 init 进程负责收集它们的退出状态。 
 */

/**
 * pid_t waitpid(pid_t pid, int *status, int options)
 * 
 * pid：
 *  pid > 0：只等待进程 ID 等于 pid 的子进程，不管已经有其他子进程运行结束退出了，只要指定的子进程还没有结束， waitpid()就会一直等下去
 *  pid = -1：等待任何一个子进程退出，此时和 wait()作用一样
 *  pid = 0：等待其组 ID 等于调用进程的组 ID 的任一子进程
 *  pid < -1：等待其组 ID 等于 pid 的绝对值的任一子进程
 * 
 * status: 是一个整型指针，是该子进程退出时的状态， status 若不为空，则通过它可以获得子进程的结束状态
 * 另外，子进程的结束状态可由 Linux 中一些特定的宏来测定
 * 
 * options:
 *  WNOHANG：若由 pid 指定的子进程不立即可用，则 waitpid()不阻塞，此时返回值为 0
 *  WUNTRACED：若实现某支持作业控制，则由 pid 指定的任一子进程状态已暂停，且其状态自暂停以来还未报告过，则返回其状态
 *  0：同 wait()，阻塞父进程，等待子进程退出
 * 
 * 返回
 *  正常：已经结束运行的子进程的进程号
 *  使用选项 WNOHANG 且没有子进程退出： 0
 *  调用出错： -1
 */


/** 
 * demo 说明：
 * 使用 fork()创建一个子进程，然后让其子进程暂停 5s（使用了 sleep()函数）。接下来对原有的父进程使用 waitpid()函数，并使用参数
 * WNOHANG 使该父进程不会阻塞。若有子进程退出，则 waitpid()返回子进程号；若没有子进程退出，则waitpid()返回 0，并且父进程每隔一秒循环判断一次。
 */


#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
    pid_t pc, pr;

    pc = fork();

    if (pc < 0)
    {
        printf("Error fork\n");
    }
    else if (pc == 0) /*子进程*/
    {
        /*子进程暂停 5s*/
        sleep(5);
        /*子进程正常退出*/
        exit(0);
    }
    else /*父进程*/
    {
        /*循环测试子进程是否退出*/
        do
        {
            /*调用 waitpid，且父进程不阻塞*/
            pr = waitpid(pc, NULL, WNOHANG);
            
            /*若子进程还未退出，则父进程暂停 1s*/
            if (pr == 0)
            {
                printf("The child process has not exited\n");
                sleep(1);
            }
        } while (pr == 0);

        /*若发现子进程退出，打印出相应情况*/
        if (pr == pc)
        {
            printf("Get child exit code: %d\n",pr);
        }
        else
        {
            printf("Some error occured.\n");
        }
        
        exit(0);
    }
}





