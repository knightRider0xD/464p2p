#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include <linux/netfilter.h>
#include <linux/netfilter_ipv6.h>
#include <linux/netfilter_ipv4.h>
#include <linux/hashtable.h>
#include <arpa/inet.h>
#include <linux/string.h>

#include "64_inbound.h"
#include "46_outbound.h"

#define 464P2P_VERBOSE

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ian Knight");

//netfilter hook options
static struct nf_hook_ops in_nfho;
static struct nf_hook_ops out_nfho;

//Module Args
static char xlatArray[] = {};
static int xlatArray_count = 0;
module_param_array(xlatArray, char[], &xlatArray_count, 0000);
//TODO MODULE_PARM_DESC(xlatArray, "A table of IPv4 to IPv6 address mappings in the format \"IPv4 Address 1\",\"IPv6 Address 1\",\"IPv4 Address 2\",\"IPv6 Address 2\",...");
MODULE_PARM_DESC(xlatArray, "A table of IPv4 to IPv6 address mappings in the format \"IPv4 Address\",\"IPv6 Address\"");

//On load using 'insmod'
int init_module() {
    
    /*/ Check for 2 strings in array (temporary limit) if not 2 strings, prevent loading of module
    if(xlatArray_count != 2){
        printk(KERN_INFO "Invalid number or Arguments\n");
        return 1;
    }
    
    if(inet_pton(AF_INET,xlatArray[0],s_464_addr)!=1){
        printk(KERN_INFO "Invalid IPv4 Address\n");
        return 1;
    }
    
    if(inet_pton(AF_INET6,xlatArray[1],d_464_addr)!=1){
        printk(KERN_INFO "Invalid IPv6 Address\n");
        return 1;
    }
    */
    
    /*
    in_nfho.hook = on_nf_hook_in;                       //function to call when conditions below met
    in_nfho.hooknum = NF_IP6_LOCAL_IN;            //After IPv6 packet routed and before local delivery
    in_nfho.pf = PF_INET;                           //IP packets
    in_nfho.priority = NF_IP6_PRI_NAT_SRC;             //set to equal priority as NAT src
    nf_register_hook(&in_nfho);                     //register hook
    
    out_nfho.hook = on_nf_hook_out;                       //function to call when conditions below met
    out_nfho.hooknum = NF_IP_LOCAL_IN;            //After IPv4 packet Created and before routing
    out_nfho.pf = PF_INET;                           //IP packets
    out_nfho.priority = NF_IP_PRI_NAT_SRC;             //set to equal priority as NAT src
    nf_register_hook(&out_nfho);                     //register hook
    */
    
    return 0;                                    //return 0 for success
}

//On unload using 'rmmod'
void cleanup_module()
{
    // remove hooks
    /*
    nf_unregister_hook(&in_nfho);
    nf_unregister_hook(&out_nfho);
    */
} 
