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

struct nf_hook_ops *reinject_4_nfho;
struct nf_queue_entry *reinject_4_qent;

int init_64_inbound(struct nf_hook_ops *nfho){
    // New packet header
    in4_hdr = kzalloc(sizeof(struct iphdr),GFP_KERNEL);
    in4_hdr->ihl         = 10; //size of IPv6 Header
    in4_hdr->version     = 4;
    //in4_hdr->check       = 0; // Ignore checksum; should have already passed checksum
    //in4_hdr->id          = 0; // Ignore packet ID; packet is unfragmented
    //in4_hdr->frag_off    = 0; // Ignore fragmentation offset & flags packet is unfragmented
    
    reinject_4_nfho = nfho;
    
    return 0;
}

// On NetFilter hook triggered
unsigned int on_nf_hook_in(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {
  
    //#ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] IN; New Packet.\n");
    //#endif
    
    in_skb = skb;
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
        printk(KERN_INFO "[464P2P] IN; Remote address not found; Dropping");
    #endif
        return NF_DROP;
    }
    
    // Collate new v4 header values
    in4_hdr->tot_len     = sizeof(struct iphdr)+in6_hdr->payload_len; // total length = header size (40 bytes + v6 payload size)
    in4_hdr->protocol    = in6_hdr->nexthdr;
    memcpy(&in4_hdr->saddr, s_4_addr,sizeof(struct in_addr));
    memcpy(&in4_hdr->daddr, d_4_addr,sizeof(struct in_addr));
    in4_hdr->ttl         = in6_hdr->hop_limit;
    in4_hdr->tos         = (in6_hdr->priority<<4) + (in6_hdr->flow_lbl[0]>>4);
    
    
    // Pull mac and network layer headers ready to push new head network layer headerRemove IPv6 header
    skb_pull(in_skb, skb_transport_offset(in_skb));
    
    // Push space for new IPv4 header
    skb_push(in_skb, sizeof(struct iphdr));
    // Reset header positions
    skb_reset_network_header(in_skb);
    skb_reset_mac_header(in_skb);
    
    // Write new v4 header data
    memcpy(skb_network_header(in_skb),in4_hdr, sizeof(struct iphdr));
    
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] IN; 6->4 XLAT Done; Moving to IPv4 queue.\n");
    #endif
    
    reinject_4_qent = kzalloc(sizeof(struct nf_queue_entry),GFP_KERNEL);
    reinject_4_qent->skb = in_skb;
    reinject_4_qent->elem = reinject_4_nfho;
    reinject_4_qent->pf = PF_INET;
    reinject_4_qent->hook = hooknum; //NF_IP_LOCAL_IN
    reinject_4_qent->indev = *(&in);
    reinject_4_qent->outdev = *(&out);
    reinject_4_qent->okfn = okfn;
    reinject_4_qent->size = sizeof(struct nf_queue_entry);
    
    nf_reinject(reinject_4_qent,NF_ACCEPT);

    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] IN; 4 Packet Reinjected, 6 Packet Stolen.\n");
    #endif
    
    return NF_STOLEN;
}

// Accept all packets for x hook
unsigned int x_nf_hook_in(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {return NF_ACCEPT;}