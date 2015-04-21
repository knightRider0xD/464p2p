

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/skbuff.h> 
#include "46_outbound.h"

// On NetFilter hook triggered
unsigned int on_nf_hook_out(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)){

    out_skb = *skb;
    out4_hdr = ip_hdr(out_skb);
    
    // XLAT v4 local address
    struct in_addr *s_6_addr = local_46_xlat(out4_hdr->saddr);
    
    // If packet src address isn't a 464p2p address, ignore packet, ACCEPT for regular processing.
    if (memcmp(out4_hdr->saddr,s_464_addr)){
#ifdef 464P2P_VERBOSE
        printk(KERN_INFO "[464P2P] OUT; Regular Packet; Passing.\n");
#endif
        return NF_ACCEPT;
    }
    
    // If packet dest address is a 464p2p address, convert packet, STOLEN and queue for v4 processing.
#ifdef 464P2P_VERBOSE
    printk(KERN_INFO "[464P2P] OUT; My Packet; Converting 4->6 ...");
#endif  
    
    // XLAT v4 remote address
    struct in_addr *d_6_addr = remote_46_xlat(out4_hdr->daddr);
    
    if(s_4_addr==NULL){
#ifdef 464P2P_VERBOSE
        printk(KERN_INFO "[464P2P] OUT; Remote address not found; Dropping");
#endif        
    }
    
    // Collate new v6 header values
    out6_hdr->payload_len     = out4_hdr->tot_len-sizeof(struct iphdr); // payload length = total length - header size
    out6_hdr->nexthdr         = out4_hdr->protocol;
    out6_hdr->saddr           = s_6_addr;
    out6_hdr->daddr           = d_6_addr;
    out6_hdr->hop_limit        = out4_hdr->ttl;
    out6_hdr->priority = (out4_hdr->tos>>4);
    out6_hdr->flow_lbl[0] = ((out4_hdr->tos&15)<<4);// + current flow label value (does not exist)
    
    // Remove IPv4 header
    skb_pull(out_skb, sizeof(struct iphdr));
    
    // Allocate IPv6 header
    //need to delete old skb after
    //skb_realloc_headroom(out_skb, sizeof(struct ipv6hdr));
    out_skb->nh.raw = skb_push(out_skb, sizeof(struct ipv6hdr));
    
    // Write new v4 header data
    memcpy(out_skb->nh.raw,out6_hdr, sizeof(struct ipv6hdr));
    
#ifdef 464P2P_VERBOSE
    printk(KERN_INFO "[464P2P] OUT; 4->6 XLAT Done; Moving to IPv6 queue.\n");
#endif
    
    ip_local_deliver(out_skb);
    
    return NF_STOLEN;
}

