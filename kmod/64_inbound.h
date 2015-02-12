struct sk_buff *in_skb;             //inbound packet
struct ipv6hdr *in_hdr;             //IP header of inbound packet
struct in6_addr *d_464_addr;

// On NetFilter hook triggered
unsigned int on_nf_hook_in(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *));