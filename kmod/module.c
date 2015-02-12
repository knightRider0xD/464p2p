#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv6.h>
#include <linux/netfilter_ipv4.h>
#include "64_inbound.h"
#include "46_outbound.h"

// Compile with -D 464P2P_VERBOSE for debugging information

static struct nf_hook_ops in_nfho;         //netfilter hook options
static struct nf_hook_ops out_nfho;         //netfilter hook options

//On load using 'insmod'
int init_module() {
    in_nfho.hook = on_nf_hook_in;                       //function to call when conditions below met
    in_nfho.hooknum = NF_IP6_LOCAL_IN;            //After IPv6 packet routed and before local delivery
    in_nfho.pf = PF_INET;                           //IP packets
    in_nfho.priority = NF_IP6_PRI_NAT_SRC;             //set to equal priority as NAT src
    nf_register_hook(&in_nfho);                     //register hook
    
    //out_nfho.hook = on_nf_hook_out;                       //function to call when conditions below met
    //out_nfho.hooknum = NF_IP_LOCAL_IN;            //After IPv6 packet routed and before local delivery
    //out_nfho.pf = PF_INET;                           //IP packets
    //out_nfho.priority = NF_IP6_PRI_NAT_SRC;             //set to equal priority as NAT src
    //nf_register_hook(&out_nfho);                     //register hook

    return 0;                                    //return 0 for success
}

//On unload using 'rmmod'
void cleanup_module()
{
    // remove hooks
    nf_unregister_hook(&in_nfho);
    //nf_unregister_hook(&out_nfho);
} 
