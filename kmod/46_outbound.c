#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/ip.h>
#include <linux/skbuff.h> 
#include <linux/ipv6.h>
#include <net/ipv6.h>
#include <net/netfilter/nf_queue.h>

#include "46_outbound.h"
#include "464_tables.h"

#define VERBOSE_464P2P

struct sk_buff *out_skb;             //inbound packet
struct iphdr *out4_hdr;             //IP header of inbound packet
struct ipv6hdr *out6_hdr;             //IP header of inbound packet;

struct in6_addr *s_6_addr;
struct in6_addr *d_6_addr;

struct flowi6 *reinject_fl6;

static int outbound_46_flowlabels = 0;
static struct ipv6_pinfo *flow_pinfo;

void set_46_flowlabels(int enable){

    flow_pinfo = kzalloc(sizeof(struct ipv6_pinfo),GFP_KERNEL);
    flow_pinfo->hop_limit = 64;
    
    if(enable){
        flow_pinfo->autoflowlabel = 1;
    }
}

// On NetFilter hook triggered
unsigned int on_nf_hook_out(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)){
    
    
    out_skb = skb;
    
    if(!out_skb){
        return NF_ACCEPT;
    }
    
    out4_hdr = ip_hdr(out_skb);

    // XLAT v4 local address
    s_6_addr = local_46_xlat((struct in_addr*) &(out4_hdr->saddr));

    // If packet src address isn't a 464p2p address, ignore packet, ACCEPT for regular processing.
    if (s_6_addr == NULL){
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] OUT; Regular Packet; ACCEPT.\n");
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
            printk(KERN_INFO "[464P2P] OUT; Remote address not found; DROP");
        #endif        
        return NF_DROP;
    }
    
    //Prep new v6 Header info
    reinject_fl6 = kzalloc(sizeof(struct flowi6),GFP_KERNEL);
    reinject_fl6->daddr = *d_6_addr;
    reinject_fl6->saddr = *s_6_addr;
    reinject_fl6->flowi6_proto = out4_hdr->protocol;
    reinject_fl6->flowi6_tos = (out4_hdr->tos>>4);
    reinject_fl6->flowlabel = 0;
    reinject_fl6->fl6_dport = 0;
    
    printk("sock: %lu, inet_sock: %lu", (unsigned long)sizeof(struct inet_sock),(unsigned long)sizeof(*out_skb->sk))
    
    //printk(KERN_INFO "[464P2P] OUT; mac size:%d;net size:%d.\n",skb_network_header(out_skb)-skb_mac_header(out_skb), skb_network_header_len(out_skb));
    
    // Pull mac and network layer headers ready to push new head network layer headerRemove IPv6 header
    skb_pull(out_skb, skb_transport_offset(out_skb));
    
    //Check if headroom expanding needed here
    if (skb_headroom(out_skb) < sizeof(struct ipv6hdr)){
        // Reallocate room for IPv6 header
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] OUT; 4->6 Expanding SKB.\n");
        #endif
        pskb_expand_head(out_skb, sizeof(struct ipv6hdr)-skb_headroom(out_skb), 0,GFP_ATOMIC);
    }
    
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] OUT; 4->6 XLAT Done; XMIT packet.\n");
    #endif
    
    ((struct inet_sock *)out_skb->sk)->pinet6 = flow_pinfo;

    if(ip6_xmit(out_skb->sk, out_skb, reinject_fl6,NULL, out4_hdr->tos) < 0){
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] OUT; Error Sending 6 Packet; DROP.\n");
        #endif
        return NF_DROP;
    }
    
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] OUT; 6 Packet XMIT OK, Mark 4 Packet STOLEN.\n");
    #endif
    
    return NF_STOLEN;
    
}