#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#ifndef NETLINK_464P2P
#define NETLINK_464P2P           24
#endif /* NETLINK_464P2P */

#define NL464_ACK               0x80000000
#define NL464_DATA4             0x40000000
#define NL464_DATA6             0x20000000

//#define NL464_LOCAL_STATUS    0x8000
#define NL464_LOCAL_ADD         0x4000
//#define NL464_LOCAL_REMOVE    0x2000

//#define NL464_REMOTE_STATUS   0x0080
#define NL464_REMOTE_ADD        0x0040
//#define NL464_REMOTE_REMOVE   0x0020

struct nl464data {
    __u32 flags;
    struct in_addr in4;
    struct in6_addr in6;
};

struct nl464data data;
int err;

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock;
struct msghdr msg;

int main(int argc, char *argv[])
{
    
    if(argc < 4){
        printf("Invalid arguments: Expected 4, recieved %d. Exiting.\n",argc);
        return -1;
    }
    
    // Determine Command
    if(strcmp(argv[0], "local")){
        if(strcmp(argv[1], "add")){
            data.flags = data.flags | NL464_LOCAL_ADD;
        } else {
            printf("Invalid command %s. Exiting.\n",argv[1]);
            return -2;
        }
    } else if(strcmp(argv[0], "remote")){
        if(strcmp(argv[1], "add")){
            data.flags = data.flags | NL464_REMOTE_ADD;
        } else {
            printf("Invalid command %s. Exiting.\n",argv[1]);
            return -2;
        }
    } else {
        printf("Invalid command %s. Exiting.\n",argv[0]);
        return -2;
    }
    
    // Get IPv4 Address
    err = inet_pton(AF_INET, argv[2], &data.in4);
    if(err<0){
        printf("Invalid IPv4 Address %s. Exiting.\n",argv[2]);
        return -5;
    }
    data.flags = data.flags | NL464_DATA4;
    
    // Get IPv6 Address
    err = inet_pton(AF_INET6, argv[3], &data.in6);
    if(err<0){
        printf("Invalid IPv6 Address %s. Exiting.\n",argv[3]);
        return -6;
    }
    data.flags = data.flags | NL464_DATA6;
    
    // Open netlink socket
    sock=socket(PF_NETLINK, SOCK_RAW, NETLINK_464P2P);
    if(sock<0){
        printf("Error opening Netlink Socket. Exiting.\n");
        return -9;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */

    bind(sock, (struct sockaddr*)&src_addr, sizeof(src_addr));

    memset(&dest_addr, 0, sizeof(dest_addr));
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(struct nl464data)));
    memset(nlh, 0, NLMSG_SPACE(sizeof(struct nl464data)));
    nlh->nlmsg_len = NLMSG_SPACE(sizeof(struct nl464data));
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    memcpy(NLMSG_DATA(nlh), &data, sizeof(struct nl464data));

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    printf("Sending message to kernel\n");
    sendmsg(sock,&msg,0);
    printf("Waiting for message from kernel\n");

    /* Read message from kernel */
    recvmsg(sock, &msg, 0);
    printf("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));
    close(sock);
}