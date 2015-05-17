
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/ip.h>
#include <linux/skbuff.h> 

#include <net/ipv6.h>
#include <net/netfilter/nf_queue.h>

#include "46_outbound.h"
#include "464_tables.h"

struct sk_buff *out_skb;             //inbound packet
struct iphdr *out4_hdr;             //IP header of inbound packet
struct ipv6hdr *out6_hdr;             //IP header of inbound packet;

struct in_addr *s_6_addr;
struct in_addr *d_6_addr;

struct nf_hook_ops *reinject_nfho;
struct nf_queue_entry *reinject_qent;

struct nf_queue_entry *reinject_qent;

int init_46_outbound(struct nf_hook_ops *nfho){
    // New packet header
    out6_hdr = kzalloc(sizeof(struct iphdr),GFP_KERNEL);
    out6_hdr->version     = 6;
    
    reinject_nfho = nfho;
    
    return 0;
}

// On NetFilter hook triggered
unsigned int on_nf_hook_out(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)){

    out_skb = *skb;
    out4_hdr = ip_hdr(out_skb);
    
    // XLAT v4 local address
    s_6_addr = local_46_xlat((struct in_addr*) &out4_hdr->saddr);
    
    // If packet src address isn't a 464p2p address, ignore packet, ACCEPT for regular processing.
    if (s_6_addr == NULL){
#ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] OUT; Regular Packet; Passing.\n");
#endif
        return NF_ACCEPT;
    }
    
    // If packet dest address is a 464p2p address, convert packet, STOLEN and queue for v4 processing.
#ifdef VERBOSE_464P2P
    printk(KERN_INFO "[464P2P] OUT; My Packet; Converting 4->6 ...");
#endif  
    
    // XLAT v4 remote address
    d_6_addr = remote_46_xlat((struct in_addr*) &out4_hdr->daddr);
    
    if(d_6_addr==NULL){
#ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] OUT; Remote address not found; Dropping");
#endif        
        return NF_DROP;
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
    skb_realloc_headroom(out_skb, sizeof(struct ipv6hdr)+32);
    //out_skb->nh.raw = skb_push(out_skb, sizeof(struct ipv6hdr));
    skb_set_network_header(out_skb,skb_push(out_skb, sizeof(struct ipv6hdr)));
    
    
    // Write new v6 header data
    memcpy(skb_network_header(out_skb),out6_hdr, sizeof(struct ipv6hdr));
    
#ifdef VERBOSE_464P2P
    printk(KERN_INFO "[464P2P] OUT; 4->6 XLAT Done; Moving to IPv6 queue.\n");
#endif
    
    reinject_qent = kzalloc(sizeof(struct nf_queue_entry),GFP_KERNEL);
    reinject_qent->skb = out_skb;
    reinject_qent->elem = reinject_nfho;
    reinject_qent->pf = PF_INET6;
    reinject_qent->hook = hooknum; //NF_IP_LOCAL_IN
    reinject_qent->indev = *(&in);
    reinject_qent->outdev = *(&out);
    reinject_qent->okfn = okfn;
    reinject_qent->size = sizeof(struct nf_queue_entry);
    
    nf_reinject(reinject_qent,NF_ACCEPT);

#ifdef VERBOSE_464P2P
    printk(KERN_INFO "[464P2P] OUT; 6 Packet Reinjected, 4 Packet Stolen.\n");
#endif
    
    return NF_STOLEN;
    
}

// Accept all packets for x hook
unsigned int x_nf_hook_out(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {return NF_ACCEPT;}