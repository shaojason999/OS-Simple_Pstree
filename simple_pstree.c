#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>

struct sockaddr_nl src_addr, des_addr;
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

    /*create a socket and assign a name to the socket*/
    sock_fd=socket(AF_NETLINK, SOCK_RAW,1);	/*created but without address assigned to it*/
    bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));	/*assign the address specified by src_addr to the sock_fd*/

    /*point to the message you want to send*/
    struct iovec iov;
    iov.iov_base="123";	/*point to the message you want to send*/
    iov.iov_len=1;/**/

    /*the actual message(package) you send to the kernel*/
    struct msghdr msg;
    msg.msg_name=(void*)&src_addr;
    msg.msg_namelen=sizeof(src_addr);
    msg.msg_iov=&iov;
    msg.msg_iovlen=1;

    sendmsg(sock_fd, &msg, 0);





}
