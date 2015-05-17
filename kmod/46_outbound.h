#ifndef __46_OUTBOUND
#define __46_OUTBOUND

int init_46_outbound(struct nf_hook_ops *nfho);

// On NetFilter hook triggered
unsigned int on_nf_hook_out(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *));

//X Protocol hook for reinjecting packets
unsigned int x_nf_hook_out(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *));

#endif /* __464_OUTBOUND */