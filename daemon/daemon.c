/**
 * 守护进程，也就是通常所说的 Daemon 进程，是 Linux 中的后台服务进程。它是一个生存期较长的进程，
 * 通常独立于控制终端并且周期性地执行某种任务或等待处理某些发生的事件。守护进程常常在系统引导载
 * 入时启动，在系统关闭时终止。 Linux 有很多系统服务，大多数服务都是通过守护进程实现的，如本书在
 * 第二章中讲到的多种系统服务都是守护进程。同时，守护进程还能完成许多系统任务，例如，作业规划进
 * 程 crond、打印进程 lqd 等（这里的结尾字母 d 就是 Daemon 的意思）。
 * 由于在 Linux 中，每一个系统与用户进行交流的界面称为终端，每一个从此终端开始运行的进程都会依附
 * 于这个终端，这个终端就称为这些进程的控制终端，当控制终端被关闭时，相应的进程都会自动关闭。但
 * 是守护进程却能够突破这种限制，它从被执行开始运转，直到整个系统关闭时才会退出。如果想让某个进
 * 程不因为用户、终端或者其他的变化而受到影响，那么就必须把这个进程变成一个守护进程。可见，守护
 * 进程是非常重要的。
 * 
 * 步骤：
 * 1．创建子进程，父进程退出
 * 2. 在子进程中创建新会话
 * 3．改变当前目录为根目录
 * 4．重设文件权限掩码 
 * 5．关闭文件描述符
 * 
 * 守护进程的出错处理：
 * 使用 syslog 服务，将程序中的出错信息输入到系统日志文件中（例如：“/var/log/messages”），从而可以直观地看到程序的问题所在。
 * syslog 是 Linux 中的系统日志管理服务，通过守护进程 syslogd 来维护。该守护进程在启动时会读一个配置
 * 文件“/etc/syslog.conf”。该文件决定了不同种类的消息会发送向何处。例如，紧急消息可被送向系统管理
 * 员并在控制台上显示，而警告消息则可被记录到一个文件中。
 * 该机制提供了 3 个 syslog 相关函数，分别为 openlog()、 syslog()和 closelog()。
 * 
 * 注意 “ /var/log/message” 系统日志文件只能由拥有 root 权限的超级用户查看。 在不同 Linux
 * 发行版本中，系统日志文件路径全名可能有所不同，例如可能是”/var/log/syslog”
 */


/* syslog_daemon.c 利用 syslog 服务的守护进程实例 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <syslog.h>
#include <sys/stat.h>

int main()
{
    pid_t pid, sid;
    int i, fd;
    char *buf = "This is a Daemon\n";

    pid = fork(); /* 第一步 */
    if (pid < 0)
    {
        printf("Error fork\n");
        exit(1);
    }

    else if (pid > 0)
    {
        exit(0); /* 父进程推出 */
    }

    /* 打开系统日志服务， openlog */
    openlog("daemon_syslog", LOG_PID, LOG_DAEMON);

    if ((sid = setsid()) < 0) /*第二步*/
    {
        syslog(LOG_ERR, "%s\n", "setsid");
        exit(1);
    }

    if ((sid = chdir("/")) < 0) /*第三步*/
    {
        syslog(LOG_ERR, "%s\n", "chdir");
        exit(1);
    }

    umask(0); /*第四步*/
    
    for(i = 0; i < getdtablesize(); i++) /*第五步*/
    {
        close(i);
    }

    /*这时创建完守护进程，以下开始正式进入守护进程工作*/
    while(1)
    {
        if((fd = open("/tmp/daemon.log", O_CREAT|O_WRONLY|O_APPEND, 0600))<0)
        {
            syslog(LOG_ERR, "open");
            exit(1);
        }

        write(fd, buf, strlen(buf) + 1);

        close(fd);

        sleep(10);
    }

    closelog();

    exit(0);
}