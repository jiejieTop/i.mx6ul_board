/** msg.c */

/**
 * 消息队列就是一些消息的列表。用户可以从消息队列中添加消息和读取消息等。
 * 消息队列的实现包括创建或打开消息队列、添加消息、读取消息和控制消息队列这 4 种操作:
 * 其中创建或打开消息队列使用的函数是 msgget()，这里创建的消息队列的数量会受到系统消息队列数量的限制；
 * 添加消息使用的函数是 msgsnd()函数，它把消息添加到已打开的消息队列末尾；
 * 读取消息使用的函数是msgrcv()，它把消息从消息队列中取走，与 FIFO 不同的是，这里可以指定取走某一条消息；
 * 最后控制消息队列使用的函数是 msgctl()，它可以完成多项功能。
 * 
 * 函数原型 int msgget(key_t key, int msgflg)
 * 函数传入值
 *   key：消息队列的键值，多个进程可以通过它访问同一个消息队列，其中有个特殊值 IPC_PRIVATE。它用于创建当前进程的私有消息队列
 *   msgflg：权限标志位
 * 函数返回值
 *   成功：消息队列 ID
 *   出错： -  1
 * 
 * 函数原型 int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)
 * 函数传入值
 * msqid：消息队列的队列 ID
 * msgp：指向消息结构的指针。该消息结构 msgbuf 通常为：
 *   struct msgbuf
 *   {
 *       long mtype;  消息类型，该结构必须从这个域开始 
 *       char mtext[1];  消息正文 
 *   }
 * msgsz：消息正文的字节数（不包括消息类型指针变量）
 * msgflg
 *   IPC_NOWAIT 若消息无法立即发送（比如：当前消息队列已满），函数会立即返回
 *   0： msgsnd 调阻塞直到发送成功为止
 * 函数返回值
 *   成功： 0
 *   出错： -1
 * 
 * 函数原型 int msgrcv(int msgid, void *msgp, size_t msgsz, long int msgtyp, int msgflg)
 * 函数传入值
 *  msgid：消息队列的队列 ID
 *  msgp：消息缓冲区, 同于 msgsnd()函数的 msgp
 *  msgsz：消息正文的字节数（不包括消息类型指针变量）
 * msgtyp：
 *   0：接收消息队列中第一个消息
 *   大于 0：接收消息队列中第一个类型为 msgtyp 的消息
 *   小于 0：接收消息队列中第一个类型值不小于 msgtyp 绝对值且类型值又最小的消息
 * msgflg：
 *   MSG_NOERROR：若返回的消息比 msgsz 字节多，则消息就会截短到 msgsz 字节，且不通知消息发送进程
 *   IPC_NOWAIT 若在消息队列中并没有相应类型的消息可以接收，则函数立即返回
 *   0： msgsnd()调用阻塞直到接收一条相应类型的消息为止
 * 函数返回值
 *   成功： 0
 *   出错： -1
 * 
 * 函数原型 int msgctl (int msgqid, int cmd, struct msqid_ds *buf )
 * 函数传入值
 * msqid：消息队列的队列 ID
 * cmd：
 *   IPC_STAT：读取消息队列的数据结构 msqid_ds，并将其存储在buf 指定的地址中
 *   IPC_SET：设置消息队列的数据结构 msqid_ds 中的 ipc_perm 域（IPC 操作权限描述结构）值。这个值取自 buf 参数
 *   IPC_RMID：从系统内核中删除消息队列
 * buf：描述消息队列的 msgqid_ds 结构类型变量
 * 函数返回值
 *   成功： 0
 *   出错： -1
 * 
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define BUFFER_SIZE 512

struct message
{
    long msg_type;
    char msg_text[BUFFER_SIZE];
};
int main()
{
    int qid;
    struct message msg;

    /*创建消息队列*/
    if ((qid = msgget((key_t)1234, IPC_CREAT|0666)) == -1)
    {
        perror("msgget");
        exit(1);
    }

    printf("Open queue %d\n",qid);

    while(1)
    {
        printf("Enter some message to the queue:");
        if ((fgets(msg.msg_text, BUFFER_SIZE, stdin)) == NULL)
        {
            puts("no message");
            exit(1);
        }
        
        msg.msg_type = getpid();

        /*添加消息到消息队列*/
        if ((msgsnd(qid, &msg, strlen(msg.msg_text), 0)) < 0)
        {
            perror("message posted");
            exit(1);
        }

        if (strncmp(msg.msg_text, "quit", 4) == 0)
        {
            break;
        }
    }

    exit(0);
}
