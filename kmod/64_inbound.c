#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/skbuff.h>
#include <linux/string.h>

#include <net/ip.h>
#include <net/netfilter/nf_queue.h>

#include "64_inbound.h"
#include "464_tables.h"

#define VERBOSE_464P2P

struct sk_buff *in_skb;             //inbound packet
struct ipv6hdr *in6_hdr;            //IPv6 header of inbound packet
struct iphdr *in4_hdr;              //New IPv4 header for inbound packet

struct in_addr *d_4_addr;
struct in_addr *s_4_addr;

struct flowi4 *reinject_fl4;

// On NetFilter hook triggered
unsigned int on_nf_hook_in(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {
  
    //#ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] IN; New Packet.\n");
    //#endif
    
    in_skb = skb;
    
    if(!in_skb){
        return NF_ACCEPT;
    }
    
    in6_hdr = ipv6_hdr(in_skb);

    // XLAT v6 local address. NULL if not listed
    d_4_addr = local_64_xlat(&in6_hdr->daddr);
    
    // If packet dest address isn't a 464p2p address, ignore packet, ACCEPT for regular processing.
    if (d_4_addr == NULL){
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] IN; Regular Packet; Passing.\n");
        #endif
        return NF_ACCEPT;
    }
    
    // If packet dest address is a 464p2p address, convert packet, STOLEN and queue for v4 processing.
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] IN; My Packet; Converting 6->4 ...");    
    #endif  
    
    // XLAT v6 remote address
    s_4_addr = remote_64_xlat(&in6_hdr->saddr);
    
    if(s_4_addr==NULL){
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] IN; Remote address not found; DROP");
    #endif
        return NF_DROP;
    }
    
    //Prep new v4 Header info
    reinject_fl4 = kzalloc(sizeof(struct flowi4),GFP_KERNEL);
    reinject_fl4->daddr = d_4_addr->s_addr;
    reinject_fl4->saddr = s_4_addr->s_addr;
    reinject_fl4->flowi4_proto = in6_hdr->nexthdr;
    reinject_fl4->flowi4_tos = (in6_hdr->priority<<4) + (in6_hdr->flow_lbl[0]>>4);
    reinject_fl4->fl4_dport = 0;
    
    // Pull mac and network layer headers ready to push new head network layer header
    skb_pull(in_skb, skb_transport_offset(in_skb));
    
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] IN; 6->4 XLAT Done; XMIT Packet.\n");
    #endif
    
    if(ip_queue_xmit(in_skb->sk, in_skb, (struct flowi *)reinject_fl4) < 0){
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] IN; Error Receiving 4 Packet; DROP.\n");
        #endif
        return NF_DROP;
    }

    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] IN; 4 Packet XMIT OK, 6 Packet STOLEN.\n");
    #endif
    
    return NF_STOLEN;
}
