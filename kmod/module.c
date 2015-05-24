#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>

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

static struct nf_hook_ops in_nfhx;
static struct nf_hook_ops out_nfhx;

//Module Args
static char *v4Addr = "000.000.000.000";
module_param(v4Addr, charp, 0000);
MODULE_PARM_DESC(v4Addr, "The IPv4 Address to map");

static char *v6Addr = "0000:0000:0000:0000:0000:0000:0000:0000";
module_param(v6Addr, charp, 0000);
MODULE_PARM_DESC(v6Addr, "The IPv6 Address to map");

struct in_addr *in4_arg;
struct in6_addr *in6_arg;
int static_table_status;

//On load using 'insmod'
int init_module() {
    
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] LOAD; Initialising.\n");
        #endif
    
    in4_arg = kzalloc(sizeof(struct in_addr), GFP_KERNEL);
    if(in4_pton(v4Addr,in4_arg)!=0){
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] LOAD; Invalid IPv4 Address Supplied. Unloading.\n");
        #endif
        kfree(in4_arg);
        return -1;
    }
    
    in6_arg = kzalloc(sizeof(struct in6_addr), GFP_KERNEL);
    if(in6_pton(v6Addr,in6_arg)!=0){
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] LOAD; Invalid IPv6 Address Supplied. Unloading.\n");
        #endif
        kfree(in4_arg);
        kfree(in6_arg);
        return -1;
    }
    
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] LOAD; Initialise 464_tables & Add local addresses.\n");
    #endif
    init_tables();
    local_xlat_add(in6_arg,in4_arg);
    
    // Load initial static entries to table
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] LOAD; Add static entries.\n");
    #endif
    static_table_status = 1;
    do {
        static_table_status = static_xlat_add();
        msleep(10);
    } while (static_table_status == 0);
    
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] LOAD; Init 64 & Register Hook.\n");
    #endif
    init_64_inbound(&in_nfhx);
    
    in_nfho.hook = on_nf_hook_in;                       //function to call when conditions below met
    in_nfho.hooknum = 1; //NF_IP6_LOCAL_IN;            //After IPv6 packet routed and before local delivery
    in_nfho.pf = PF_INET6;                           //IP packets
    in_nfho.priority = 100;//NF_IP6_PRI_NAT_SRC;             //set to equal priority as NAT src
    nf_register_hook(&in_nfho);                     //register hook
    
    in_nfhx.hook = x_nf_hook_in;                       //function to call when conditions below met
    in_nfhx.hooknum = 1; //NF_IP_LOCAL_IN;            //After IPv6 packet routed and before local delivery
    in_nfhx.pf = PF_INET;                           //IP packets
    in_nfhx.priority = 100;//NF_IP_PRI_NAT_SRC;             //set to equal priority as NAT src
    nf_register_hook(&in_nfhx);                     //register hook
    
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] LOAD; Init 46 & Register Hook.\n");
    #endif
    init_46_outbound(&out_nfhx);
    
    out_nfho.hook = on_nf_hook_out;                       //function to call when conditions below met
    out_nfho.hooknum = 3; //NF_IP_LOCAL_OUT;            //After IPv4 packet Created and before routing
    out_nfho.pf = PF_INET;                           //IP packets
    out_nfho.priority = 100; //NF_IP_PRI_NAT_SRC;             //set to equal priority as NAT src
    nf_register_hook(&out_nfho);                     //register hook

    out_nfhx.hook = x_nf_hook_out;                       //function to call when conditions below met
    out_nfhx.hooknum = 3; //NF_IP6_LOCAL_OUT;            //After IPv4 packet Created and before routing
    out_nfhx.pf = PF_INET6;                           //IP packets
    out_nfhx.priority = 100; //NF_IP_PRI_NAT_SRC;             //set to equal priority as NAT src
    nf_register_hook(&out_nfhx);                     //register hook

    
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
    nf_unregister_hook(&in_nfhx);
    nf_unregister_hook(&out_nfhx);
    
    #ifdef VERBOSE_464P2P
        printk(KERN_INFO "[464P2P] UNLOAD; Housekeeping.\n");
    #endif
    
    //Clean XLAT tables
    cleanup_tables();
    // Free memory
    kfree(in4_arg);
    kfree(in6_arg);
    
} 
