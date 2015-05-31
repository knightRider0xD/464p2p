#ifndef __46_OUTBOUND
#define __46_OUTBOUND

void init_46_outbound(int enable_46_flowlabels);

// On NetFilter hook triggered
unsigned int on_nf_hook_out(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *));

#endif /* __464_OUTBOUND */