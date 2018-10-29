#include <linux/module.h>
#include <linux/kernel.h>
#include <net/sock.h>
#include <net/netlink.h>
#include "ksimple_pstree.h"
MODULE_LICENSE("GPL");

#define MAX_PAYLOAD 1024
struct sock *nl_sk=NULL;

static void recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh=NULL;

    nlh=(struct nlmsghdr *)skb->data;

    printk(KERN_INFO "%s: received netlink message payload: %s\n",__FUNCTION__, (char*)NLMSG_DATA(nlh));
}

static int __init netlink_init(void)
{
    struct netlink_kernel_cfg cfg = {
        .input=recv_msg,
    };

    printk(KERN_INFO "2 kernel say hello");
    nl_sk=netlink_kernel_create(&init_net, user_kernel_link, &cfg);
    if(!nl_sk) {
        printk(KERN_INFO "Error creating socket\n");
        return -10;
    }

    return 0;

}
static void __exit netlink_exit(void)
{
    printk(KERN_INFO "3 goodbye");
}
module_init(netlink_init);
module_exit(netlink_exit);

