#include <linux/netlink.h>
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

void on_netlink_receive(struct sk_buff *skb)
{
    
    struct nlmsghdr nlh_cache;
    struct nlmsghdr *nlh;
    
    struct nl464data nl464d_cache;
    struct nl464data *nl464d;
    
    int src_pid;
    
    struct sk_buff *skb_out;
    __u32 reponse; 
    int res;
    
    // Validate packet long enough for headers
    if(skb->data_len< sizeof(nlmsghdr)+sizeof(nl464data)){
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] NETLINK; Packet too short; Ignoring.\n");
        #endif
        return;
    }
    
    // get pointers to netlink headers
    nlh=(struct nlmsghdr*) skb_header_pointer(skb, 0, sizeof(nlmsghdr) , &nlh_cache);
    nl464d=(struct nl464head*)skb_header_pointer(skb, 0, sizeof(nl464head), &nl464h_cache);
    
    /*
    if(!nl464d->flags & NL464_DATA){
        // status request; ignore packet data fields & check status flags
        if(nl464d->flags & NL464_LOCAL_STATUS){
        
        } else if(nl464h->flags & NL464_REMOTE_STATUS){
            
        }
    }
    
    // check flags
    if(nl464d->flags & NL464_LOCAL_STATUS){
        
    } else if(nl464d->flags & NL464_REMOTE_STATUS){
        
    } else */
    if(nl464d->flags & NL464_LOCAL_ADD){
        local_xlat_add(&(nl464d->in6), &(nl464d->in4));
    } else if(nl464d->flags & NL464_REMOTE_ADD){
        remote_xlat_add(&(nl464d->in6), &(nl464d->in4));
    }/* else if(nl464d->flags & NL464_LOCAL_REMOVE){
        
    } else if(nl464d->flags & NL464_REMOTE_REMOVE){
        
    }*/
    
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


