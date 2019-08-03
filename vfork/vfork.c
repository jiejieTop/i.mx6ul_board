/* vfork.c */

/** 
 * 由于 fork()完整地复制了父进程的整个地址空间，因此执行速度是比较慢的。为
 * 了加快 fork()的执行速度，有些 UNIX 系统设计者创建了 vfork()。 vfork()也能创
 * 建新进程，但它不产生父进程的副本。它是通过允许父子进程可访问相同物理内
 * 存从而伪装了对进程地址空间的真实拷贝，当子进程需要改变内存中数据时才复
 * 制父进程。这就是著名的“写操作时复制”（ copy-on-write）技术。
 * 现在很多嵌入式 Linux 系统的 fork()函数调用都采用 vfork()函数的实现方式，实
 * 际上 uClinux 所有的多进程管理都通过 vfork()来实现。
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    pid_t result;
    /*调用 fork()函数*/
    result = vfork();
    /*通过 result 的值来判断 fork()函数的返回情况，首先进行出错处理*/
    if(result == -1)
    {
        printf("Fork error\n");
    }
    else if (result == 0) /*返回值为 0 代表子进程*/
    {
        printf("The returned value is %d\nIn child process!!\nMy PID is %d\n\n",result,getpid());

    }
    else /*返回值大于 0 代表父进程*/
    {
        printf("The returned value is %d\nIn father process!!\nMy PID is %d\n\n",result,getpid());
    }
    exit(1);
}
