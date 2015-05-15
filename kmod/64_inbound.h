#ifndef __64_INBOUND
#define __64_INBOUND

int init_64_inbound(struct nf_hook_ops *nfho);

// On NetFilter hook triggered
unsigned int on_nf_hook_in(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *));

//X Protocol hook for reinjecting packets
unsigned int x_nf_hook_in(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *));

#endif /* __64_INBOUND */