/*#include <linux/stddef.h>
//#include <linux/hashtable.h>
//#include <linux/crc32.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/kernel.h>
*/

/*
 * Included in 464_tables.h
 * 
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <uapi/linux/in.h>
#include <uapi/linux/in6.h>
*/

#include "464_netlink.h"
#include "464_tables.h"


/********************************
 * Init functions for XLAT table
 ********************************/

/*
struct xlat_entry out_addr_cache;
struct xlat_entry in_addr_cache;

struct in_addr *in4;
struct in6_addr *in6;

char addr_4[16];
char addr_6[40];
*/

struct sock * nlsock;

void netlink_receive(struct sock *sk, int len)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh = NULL;
    u8 *payload = NULL;

    while ((skb = skb_dequeue(&sk->receive_queue))
        != NULL) {
    /* process netlink message pointed by skb->data */
    nlh = (struct nlmsghdr *)skb->data;
    payload = NLMSG_DATA(nlh);
    /* process netlink message with header pointed by
    * nlh	and payload pointed by payload
    */
    }
}

int netlink_init()
{
    
    nlsock = netlink_kernel_create(NETLINK_464P2P,input);
    
    return 0;
}

int netlink_cleanup()
{
    return 0;
}


