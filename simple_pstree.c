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
struct msghdr msg;	/*have to be a global variable*/
struct iovec iov;
int sock_fd;

int main(int argc, char* argv[])
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

    /*create a socket and assign a name to the socket*/
    sock_fd=socket(AF_NETLINK, SOCK_RAW, user_kernel_link);	/*created but without address assigned to it*/
    if(sock_fd<0) {
        printf("failed to connect to kernel(may not insmod yet)\n");
        return -1;
    }
    bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));	/*assign the address specified by src_addr to the sock_fd*/

    nlh_d=(struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh_d, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh_d->nlmsg_len=NLMSG_SPACE(MAX_PAYLOAD);
    nlh_d->nlmsg_pid=getpid();
    nlh_d->nlmsg_flags=0;

    char message[50];
    memset(message, 0, sizeof(message));
    int option;
    if(argc==1)
        sprintf(message,"c 1");
    else if((option=getopt(argc, argv, "c::s::p::"))!=-1) {
        switch(option) {
        case 'c':
            if(optarg==NULL)
                sprintf(message,"c 1");
            else
                sprintf(message,"c %s",optarg);
            break;
        case 's':
            if(optarg==NULL)
                sprintf(message,"s %d",getpid());
            else
                sprintf(message,"s %s",optarg);
            break;
        case 'p':
            if(optarg==NULL)
                sprintf(message,"p %d",getpid());
            else
                sprintf(message,"p %s",optarg);
            break;
        case '?':
            printf("unknown input\n");
            return -1;
        }
    }
    strncpy(NLMSG_DATA(nlh_d), message, strlen(message));

    iov.iov_base=(void*)nlh_d;	/*point to the message you want to send*/
    iov.iov_len=nlh_d->nlmsg_len;/**/

    /*the actual message(package) you send to the kernel*/
    msg.msg_name=(void*)&des_addr;
    msg.msg_namelen=sizeof(des_addr);
    msg.msg_iov=&iov;
    msg.msg_iovlen=1;

    printf("start to send to kernel\n");
    sendmsg(sock_fd, &msg, 0);
    printf("waiting from kernel\n");

    memset(nlh_d, 0, NLMSG_SPACE(MAX_PAYLOAD));

    recvmsg(sock_fd, &msg, 0);
    if(strcmp(((char*)NLMSG_DATA(nlh_d)),"-1")!=0)	/*-1 means pid does not exist*/
        printf("Received data from kernel: %s\n", (char*)NLMSG_DATA(nlh_d));
    else
        printf("pid does not exist\n");

    close(sock_fd);

    return 0;





}
