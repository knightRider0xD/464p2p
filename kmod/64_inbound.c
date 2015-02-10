

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv6.h>
#include <linux/ip.h>
#include <linux/skbuff.h>

static struct nf_hook_ops nfho;         //netfilter hook options
struct sk_buff *in_sk_buff;             //inbound packet
struct iphdr *in_ip_header;             //IP header of inbound packet

// Called when netfilter hook called
unsigned int on_nf_hook(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *))
{
  
  if (0==0){
#ifdef 464P2P_VERBOSE
    printk(KERN_INFO "Regular Packet: Passing.\n");
#endif
    return NF_ACCEPT;
  }
  
#ifdef 464P2P_VERBOSE
    printk(KERN_INFO "464P2P Packet: Moving to IPv4 queue.\n");
#endif  
  return NF_STOLEN;
}

//Called when module loaded using 'insmod'
int init_module()
{
  nfho.hook = on_nf_hook;                       //function to call when conditions below met
  nfho.hooknum = NF_IP6_LOCAL_IN;            //called right after packet recieved, first hook in Netfilter
  nfho.pf = PF_INET;                           //IPV4 packets
  nfho.priority = NF_IP6_PRI_NAT_SRC;             //set to equal priority as NAT src
  nf_register_hook(&nfho);                     //register hook

  return 0;                                    //return 0 for success
}

//Called when module unloaded using 'rmmod'
void cleanup_module()
{
    nf_unregister_hook(&nfho);                     //cleanup – unregister hook
  
}