#include <linux/netfilter.h>
#include <linux/netfilter_ipv6.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/skbuff.h>
#include <string.h>
#include "64_inbound.h"

// On NetFilter hook triggered
unsigned int on_nf_hook_in(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {
  
    in_skb = *skb;
    in_hdr = ipv6_hdr(in_skb);
    
    d_464_addr = { { { 0xfe,0x80,0,0,0,0,0,0,0x02,0x50,0x56,0xff,0xfe,0xc0,0,0x09 } } }; //fe80::250:56ff:fec0:9
    
    
    // If packet dest address isn't a 464p2p address, ignore packet, ACCEPT for regular processing.
    if (memcmp(in_hdr->daddr,d_464_addr)){
#ifdef 464P2P_VERBOSE
        printk(KERN_INFO "Regular Packet: Passing.\n");
#endif
        return NF_ACCEPT;
    }
  
    // If packet dest address is a 464p2p address, convert packet, STOLEN and queue for v4 processing.
#ifdef 464P2P_VERBOSE
    printk(KERN_INFO "464P2P Packet: Moving to IPv4 queue.\n");
#endif  
    return NF_STOLEN;
}