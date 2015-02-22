#include <linux/netfilter.h>
#include <linux/netfilter_ipv6.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/skbuff.h>
#include <linux/string.h>
#include <net/ip.h>
#include "64_inbound.h"
#include "46_outbound.h"


// New packet header
in4_hdr = (struct iphdr*) kcalloc(40);
in4_hdr->ihl         = 10; //size of IPv6 Header
in4_hdr->version     = 4;
//in4_hdr->check       = 0; // Ignore checksum should have already passed checksum
//in4_hdr->id          = 0; // Ignore packet ID packet is unfragmented
//in4_hdr->frag_off    = 0; // Ignore fragmentation offset & flags packet is unfragmented

// On NetFilter hook triggered
unsigned int on_nf_hook_in(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {
  
    in_skb = *skb;
    in6_hdr = ipv6_hdr(in_skb);
    
    // If packet dest address isn't a 464p2p address, ignore packet, ACCEPT for regular processing.
    if (memcmp(in6_hdr->daddr,d_464_addr)){
#ifdef 464P2P_VERBOSE
        printk(KERN_INFO "Regular Packet: Passing.\n");
#endif
        return NF_ACCEPT;
    }
    
    // If packet dest address is a 464p2p address, convert packet, STOLEN and queue for v4 processing.
#ifdef 464P2P_VERBOSE
    printk(KERN_INFO "464P2P Packet: Converting 6->4 ...");
#endif  
    
    // XLAT v6 addresses
    struct in_addr *s_4_addr = remote_64_xlat(in6_hdr->saddr);
    struct in_addr *d_4_addr = local_64_xlat(in6_hdr->daddr);
    
    // Collate new v4 header values
    in4_hdr->tot_len     = 40+in6_hdr->payload_len; // total length = header size (40 bytes + v6 payload size)
    in4_hdr->protocol    = in6_hdr->nexthdr;
    in4_hdr->saddr       = s_4_addr;
    in4_hdr->daddr       = d_4_addr;
    in4_hdr->ttl         = in6_hdr->hop_limit;
    in4_hdr->tos         = (in6_hdr->priority<<4) + (in6_hdr->flow_lbl[0]>>4);
    
    // Write new v4 header over v6 header
    memcpy(in6_hdr,in4_hdr,40);
    
#ifdef 464P2P_VERBOSE
    printk(KERN_INFO "Moving to IPv4 queue.\n");
#endif
    
    ip_local_deliver(in_skb);
    
    return NF_STOLEN;
}

/**
 * Function to translate local IPv6 Address to its local IPv4 address
 */
in_addr local_64_xlat(in6_addr *local_6_addr){
    return s_464_addr;
}

/**
 * Function to translate remote IPv6 Address to a corresponding remote IPv4 address
 */
in_addr remote_64_xlat(in6_addr *remote_6_addr){
    struct in_addr *remote_4_addr;
    inet_pton(AF_INET,"192.168.5.1",remote_4_addr);
    return remote_4_addr;
}