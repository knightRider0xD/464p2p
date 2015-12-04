#include <linux/netlink.h>
#include <uapi/linux/netlink.h>
#include <net/sock.h>
#include <linux/skbuff.h>

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

#define VERBOSE_464P2P

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

struct sock * nlsock = NULL;

struct nl464data {
    __u32 flags;
    struct in_addr in4;
    struct in6_addr in6;
};

struct nlmsghdr *nlh;
struct nl464data *nl464d;

struct in_addr *nl464_in4;
struct in6_addr *nl464_in6;

int src_pid;

struct sk_buff *skb_out;
__u32 reponse; 
int res;

void on_netlink_receive(struct sk_buff *skb)
{
    
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] NETLINK; Packet Received.\n");
    #endif

    res = 1;
    
    // Validate packet long enough for headers
    /*if(skb->data_len< sizeof(struct nlmsghdr)+sizeof(struct nl464data)){
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] NETLINK; Packet too short; Ignoring.\n");
        #endif
        return;
    }*/
    
    // get pointers to netlink headers
    nlh=(struct nlmsghdr*) skb->data;
    nl464d=(struct nl464data*)nlmsg_data(nlh);
    
    // get pid of sending process
    src_pid = nlh->nlmsg_pid;
    
    
    if(!((nl464d->flags & NL464_DATA4)&&(nl464d->flags & NL464_DATA6))){
        //no addresses present
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] NETLINK; Missing Packet Data; Ignoring.\n");
        #endif
        return;
    }
    
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] NETLINK; Flags %#010x, IPv4 %pI4 , IPv6 %pI6 .\n",nl464d->flags,&(nl464d->in4),&(nl464d->in6));
    #endif
    
    
    if(nl464d->flags & NL464_LOCAL_ADD){
        nl464_in4 = kzalloc(sizeof(struct in_addr),GFP_ATOMIC);
        memcpy(nl464_in4,&(nl464d->in4),sizeof(struct in_addr));
        
        nl464_in6 = kzalloc(sizeof(struct in6_addr),GFP_ATOMIC);
        memcpy(nl464_in6,&(nl464d->in6),sizeof(struct in6_addr));
        
        res = local_xlat_add(nl464_in6, nl464_in4);
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] NETLINK; Added Local Entry.\n");
        #endif
    } else if(nl464d->flags & NL464_REMOTE_ADD){
        nl464_in4 = kzalloc(sizeof(struct in_addr),GFP_ATOMIC);
        memcpy(nl464_in4,&(nl464d->in4),sizeof(struct in_addr));
        
        nl464_in6 = kzalloc(sizeof(struct in6_addr),GFP_ATOMIC);
        memcpy(nl464_in6,&(nl464d->in6),sizeof(struct in6_addr));
        
        res = remote_xlat_add(nl464_in6, nl464_in4);
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] NETLINK; Added Remote Entry.\n");
        #endif
    }
    
    skb_out = nlmsg_new(sizeof(struct nl464data),0);

    if(!skb_out)
    {
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] NETLINK; Failed to Allocate Reply SKB.\n");
        #endif
        return;

    }
    
    nlh=nlmsg_put(skb_out,0,0,NLMSG_DONE,sizeof(struct nl464data),0);  
    NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
    memcpy(nlmsg_data(nlh),nl464d,sizeof(struct nl464data));

    res=nlmsg_unicast(nlsock,skb_out,src_pid);

    if(res<0){
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] NETLINK; Failed to Send Reply.\n");
        #endif
    }
    
}

int init_netlink()
{
    
    // Setup struct holding netlink config
    struct netlink_kernel_cfg cfg = {
        .input = on_netlink_receive,
    };
    
    // Connect netlink socket
    nlsock = netlink_kernel_create(&init_net, NETLINK_464P2P,&cfg);
    
    // Check for errors
    if(!nlsock) {
        printk(KERN_ALERT "[464P2P] Netlink; Error creating socket.\n");
        return -1;
    }
    
    return 0;
}

int cleanup_netlink()
{
    netlink_kernel_release(nlsock);
    return 0;
}


