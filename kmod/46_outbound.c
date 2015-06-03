#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/ip.h>
#include <linux/skbuff.h> 
#include <linux/ipv6.h>
#include <net/ipv6.h>
#include <net/ip6_route.h>
#include <net/net_namespace.h>
#include <net/netfilter/nf_queue.h>

#include "46_outbound.h"
#include "464_tables.h"

#define VERBOSE_464P2P

struct sk_buff *out_skb;             //inbound packet
struct iphdr *out4_hdr;             //IP header of inbound packet
struct ipv6hdr *out6_hdr;             //IP header of inbound packet;

struct in6_addr *s_6_addr;
struct in6_addr *d_6_addr;
struct flowi6 fl6;

static int outbound_46_flowlabels = 0;

void init_46_outbound(int enable_46_flowlabels){

    if(enable_46_flowlabels){
        outbound_46_flowlabels = 1;
    }
    
    // New packet header
    out6_hdr = kzalloc(sizeof(struct iphdr),GFP_ATOMIC);
    out6_hdr->version     = 6;
    
}

// On NetFilter hook triggered
unsigned int on_nf_hook_out(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)){
    
    
    if(!skb){
        return NF_ACCEPT;
    }
    
    out4_hdr = ip_hdr(skb);

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
    
    // Collate new v6 header values
    out6_hdr->payload_len      = out4_hdr->tot_len-sizeof(struct iphdr); // payload length = total length - header size
    out6_hdr->nexthdr          = out4_hdr->protocol;
    out6_hdr->saddr            = *s_6_addr;
    out6_hdr->daddr            = *d_6_addr;
    out6_hdr->hop_limit        = out4_hdr->ttl;
    out6_hdr->priority         = (out4_hdr->tos>>4);
    out6_hdr->flow_lbl[0]      = ((out4_hdr->tos&15)<<4) + ip6_make_flowlabel(sock_net(out_skb->sk), out_skb, 0,outbound_46_flowlabels); //current flow label value (does not exist)
    
    struct flowi6 fl6 = {
        .saddr = *s_6_addr,
        .daddr = *d_6_addr,
    };
    
    out_skb = skb_clone(skb, GFP_ATOMIC);
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
    
    // Push space for new IPv6 header
    skb_push(out_skb, sizeof(struct ipv6hdr));
    // Realign header positions
    skb_reset_network_header(out_skb);
    // skb_reset_mac_header(out_skb);
    
    // Write new v6 header data
    memcpy(skb_network_header(out_skb),out6_hdr, sizeof(struct ipv6hdr));
    
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] OUT; 4->6 XLAT Done; Dispatch packet.\n");
    #endif
    
    skb_scrub_packet(out_skb, 1); //scrub old connection information
        
    skb_dst_set(out_skb, ip6_route_output(&init_net, NULL, &fl6));
    
    if(ip6_local_out(out_skb) < 0){
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] OUT; Error Sending 6 Packet; DROP.\n");
        #endif
        return NF_DROP;
    }
    
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] OUT; 6 Packet XMIT OK, Mark 4 Packet STOLEN.\n");
    #endif
    
    kfree_skb(skb);
    return NF_STOLEN;
    
}