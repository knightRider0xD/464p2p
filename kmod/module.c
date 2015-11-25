#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>

#include <linux/inet.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv6.h>
#include <linux/netfilter_ipv4.h>
#include <linux/string.h>
#include <linux/delay.h>

#include "64_inbound.h"
#include "46_outbound.h"
#include "464_tables.h"

#define VERBOSE_464P2P

// Compile with -D 464P2P_VERBOSE for debugging information

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ian Knight");

//netfilter hook options
static struct nf_hook_ops in_nfho;
static struct nf_hook_ops out_nfho;

//Module Args
static int outboundfl = 0;
module_param(outboundfl, int, 0);
MODULE_PARM_DESC(outboundfl, "enables outbound IPv6 flow labels");

//On load using 'insmod'
int init_module() {
    
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] LOAD; Initialising.\n");
    #endif
    init_tables();
    init_netlink();
        
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] LOAD; Init 64 & Register Hook.\n");
    #endif
    
    in_nfho.hook = on_nf_hook_in;                       //function to call when conditions below met
    in_nfho.hooknum = 1; //NF_IP6_LOCAL_IN;            //After IPv6 packet routed and before local delivery
    in_nfho.pf = PF_INET6;                           //IP packets
    in_nfho.priority = 100;//NF_IP6_PRI_NAT_SRC;             //set to equal priority as NAT src
    nf_register_hook(&in_nfho);                     //register hook

    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] LOAD; Init 46 & Register Hook.\n");
    #endif
    
    init_46_outbound(outboundfl); //init & set flowlabel support
    
    out_nfho.hook = on_nf_hook_out;                       //function to call when conditions below met
    out_nfho.hooknum = 3; //NF_IP_LOCAL_OUT;            //After IPv4 packet Created and before routing
    out_nfho.pf = PF_INET;                           //IP packets
    out_nfho.priority = 100; //NF_IP_PRI_NAT_SRC;             //set to equal priority as NAT src
    nf_register_hook(&out_nfho);                     //register hook


    
    return 0;                                    //return 0 for success
}

//On unload using 'rmmod'
void cleanup_module()
{
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] UNLOAD; Unregister NF Hooks.\n");
    #endif
    // remove hooks
    nf_unregister_hook(&in_nfho);
    nf_unregister_hook(&out_nfho);
    
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] UNLOAD; Housekeeping.\n");
    #endif
    //Cleanup tables & netlink
    cleanup_netlink();
    cleanup_tables();
    
} 