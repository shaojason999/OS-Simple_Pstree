#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>
#include "simple_pstree.h"

#define MAX_PAYLOAD 1024
struct sockaddr_nl src_addr, des_addr;
struct nlmsghdr *nlh_d;	/*header+data*/
struct iovec iov;
int sock_fd;

int main()
{
    /*set the src_addr*/
    memset(&src_addr, 0, sizeof(src_addr));	/*<string.h>*/
    src_addr.nl_family=AF_NETLINK;
    src_addr.nl_pad=0;
    src_addr.nl_pid=getpid();	/*getpid() included in <unistd.h>*/
    src_addr.nl_groups=0;
    /*set the des_addr*/
    memset(&des_addr, 0, sizeof(des_addr));
    des_addr.nl_family=AF_NETLINK;
    des_addr.nl_pad=0;
    des_addr.nl_pid=0;	/*pid of kernel is 0*/
    des_addr.nl_groups=0;
    printf("123\n");
    /*create a socket and assign a name to the socket*/
    sock_fd=socket(AF_NETLINK, SOCK_RAW, user_kernel_link);	/*created but without address assigned to it*/
    if(sock_fd<0) {
        printf("222\n");
        return -1;
    }
    printf("345\n");
    bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));	/*assign the address specified by src_addr to the sock_fd*/

    nlh_d=(struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh_d, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh_d->nlmsg_len=NLMSG_SPACE(MAX_PAYLOAD);
    nlh_d->nlmsg_pid=getpid();
    nlh_d->nlmsg_flags=0;
    strcpy(NLMSG_DATA(nlh_d), "hello!");
    printf("234\n");
    iov.iov_base=(void*)(nlh_d);	/*point to the message you want to send*/
    iov.iov_len=nlh_d->nlmsg_len;/**/

    /*the actual message(package) you send to the kernel*/
    struct msghdr msg;
    msg.msg_name=(void*)&des_addr;
    msg.msg_namelen=sizeof(des_addr);
    msg.msg_iov=&iov;
    msg.msg_iovlen=1;

    printf("start to send to kernel\n");
    sendmsg(sock_fd, &msg, 0);
    printf("waiting from kernel\n");

    return 0;





}
