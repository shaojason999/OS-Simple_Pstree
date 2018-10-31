#include <linux/module.h>
#include <net/sock.h>
#include <net/netlink.h>
#include "ksimple_pstree.h"
#include <linux/skbuff.h>
#include <linux/pid.h>

#define MAX_PAYLOAD 1024
struct sock *nl_sk=NULL;
char msg[100000], buff[100];
int depth;

void parent(struct task_struct *task)
{
    int i;
    struct task_struct *temp_task;
    if(((temp_task=task->parent)->pid)!=0) {
        parent(temp_task);
        for(i=0; i<depth; ++i)
            strcat(msg,"    ");
    }
    sprintf(buff,"%s(%d)\n", task->comm, task->pid);
    strcat(msg,buff);
    ++depth;
}
void child(struct task_struct *task)
{
    struct list_head *list=NULL;
    struct task_struct *temp_task;
    int i;
    for(i=0; i<depth; ++i)
        strcat(msg,"    ");
    sprintf(buff,"%s(%d)\n", task->comm, task->pid);
    printk("%s(%d)\n", task->comm, task->pid);
    strcat(msg,buff);
    list_for_each(list, &(task->children)) {
        temp_task=list_entry(list, struct task_struct, sibling);
        ++depth;
        child(temp_task);
        --depth;
    }
}
static void recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh_recv,*nlh_send;
    struct sk_buff *skb_out;
    int back_pid, res, msg_size;
    int pid, ori_pid;

    char *msg_recv;
    struct pid *f_g_pid;
    struct task_struct *task, *temp_task;
    struct list_head *list;

    /*deal with the data received*/
    nlh_recv=(struct nlmsghdr *)skb->data;
    printk(KERN_INFO "%s: received message from user: %s\n",__FUNCTION__, (char*)NLMSG_DATA(nlh_recv));
    back_pid=nlh_recv->nlmsg_pid;	/*pid from user process*/

    memset(msg, 0, sizeof(msg));
    memset(buff, 0, sizeof(buff));
    list=NULL;
    depth=0;
    msg_recv=(char*)NLMSG_DATA(nlh_recv);
    sscanf(&msg_recv[2],"%d",&pid);
    printk("pid: %d\n",pid);
    f_g_pid=find_get_pid(pid);
    if(f_g_pid==NULL) {	/*if pid does not exist, send back "-1"*/
        memset(msg, 0, sizeof(msg));
    } else {
        task=pid_task(f_g_pid,PIDTYPE_PID);
        if(msg_recv[0]=='s') {
            ori_pid=task->pid;
            task=task->parent;
            list_for_each(list,&(task->children)) {
                temp_task=list_entry(list, struct task_struct, sibling);
                if(temp_task->pid != ori_pid) {
                    sprintf(buff,"%s(%d)\n",temp_task->comm, temp_task->pid);
                    strcat(msg,buff);
                }
            }
        } else if(msg_recv[0]=='p') {
            parent(task);
        } else if(msg_recv[0]=='c') {
            child(task);
        }

    }

    /*prepare to send the data*/
//    msg="say hello from kenrel";

    msg_size=strlen(msg);
    skb_out=nlmsg_new(msg_size,0);
    if(!skb_out) {
        printk(KERN_ERR "failed to open a new sk_buffer\n");
        return;
    }

    nlh_send=nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    strncpy(NLMSG_DATA(nlh_send), msg, msg_size);

    NETLINK_CB(skb_out).dst_group=0;

    /*send the data*/
    res=nlmsg_unicast(nl_sk, skb_out, back_pid);
    if(res<0)
        printk(KERN_INFO "Error while sending back to user\n");
    return;

}

static int __init netlink_init(void)
{
    struct netlink_kernel_cfg cfg = {
        .input=recv_msg,
    };

    printk("kernel say hello\n");
    nl_sk=netlink_kernel_create(&init_net, user_kernel_link, &cfg);
    if(!nl_sk) {
        printk(KERN_ALERT "Error creating socket\n");
        return -10;
    }

    return 0;

}
static void __exit netlink_exit(void)
{
    printk(KERN_ALERT "goodbye\n");
    netlink_kernel_release(nl_sk);
}
module_init(netlink_init);
module_exit(netlink_exit);


MODULE_LICENSE("GPL");
