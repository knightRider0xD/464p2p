#include <linux/stddef.h>
#include <linux/sysctl.h>
#include <linux/hastable.h>
#include <linux/linkedlist.h>
#include <linux/string.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <uapi/linux/in.h>
#include <uapi/linux/in6.h>
#include "464_tables.h"

/********************************
 * Init functions for XLAT table
 ********************************/

char static_table[] = "";
char dynamic_table[] = "";
char static_entry[56] = "";
char dynamic_entry[56] = "";

xlat_entry out_addr_cache;
xlat_entry in_addr_cache;


/* two integer items (files) */
static ctl_table net_464p2p_table[] = {
   {CTL_UNNUMBERED, "static_table", &static_table,102400, 0444, NULL, &proc_dostring,NULL, /* fill with 0's */},
   {CTL_UNNUMBERED, "dynamic_table", &dynamic_table,1024000, 0444, NULL, &proc_dostring,NULL, /* fill with 0's */},
   {CTL_UNNUMBERED, "static_entry", &static_entry,56, 0644, NULL, &proc_dostring,NULL, /* fill with 0's */},
   {CTL_UNNUMBERED, "dynamic_entry", &dynamic_entry,56, 0644, NULL, &proc_dostring,NULL, /* fill with 0's */},
   {0}
   };

/* a directory */
static ctl_table net_table[] = {
        {CTL_UNNUMBERED, "464p2p", NULL, 0, 0555,
            net_464p2p_table},
        {0}
    };

/* the parent directory */
static ctl_table root_table[] = {
        {CTL_NET, "net", NULL, 0, 0555,
            net_table},
        {0}
    }; 

static struct ctl_table_header *ctl_table_header;

int init_tables(void)
{
    ctl_table_header = register_sysctl_table(root_table,0);
    return ctl_table_header;
   
   
    DEFINE_HASHTABLE(xlat_46, 16);
    DEFINE_HASHTABLE(xlat_64, 16);

    LIST_HEAD(xlat_local);

    
}

void cleanup_tables(void)
{
    unregister_sysctl_table(ctl_table_header);
}

/********************************
 * XLAT Tables
 ********************************/

/**
 * Function to parse XLAT Entry from string
 */
int xlat_add(char xlat_str[]){
    
    char addr_6[40];
    strncpy(addr_6,xlat_str,39);
    
    in6_addr in6 = kmalloc(sizeof in6_addr,);
    
    if(inet_pton(AF_INET6,addr_6,in6)<=0){
        kfree(in6);
        return 1;
    }
    
    
    char addr_4[16]
    strncpy(addr_4,xlat_str+40,15);
    
    in_addr in4 = kmalloc(sizeof in_addr,);
    
    if(inet_pton(AF_INET,addr_4,in4)<=0){
        kfree(in6);
        kfree(in4);
        return 2;
    }
    
    return remote_xlat_add(in6, in4);
    
}

/**
 * Function to parse static XLAT Entry
 */
int static_xlat_add(){
    
    return xlat_add(static_entry);
    
}

/**
 * Function to parse dynamic XLAT Entry
 */
int dynamic_xlat_add(){
    
    return xlat_add(dynamic_entry);
    
}


/**
 * Function to add entry to the remote xlat table
 */
int remote_xlat_add(in6_addr *remote_6_addr, in_addr *remote_4_addr){
    
    struct xlat_entry new_entry_46 {
        .in6 = remote_6_addr;
        .in4 = remote_4_addr;
        .hash_list_data = 0; // initialised when added to hashtable
    };
    
    hash_add(xlat_46, &new_entry_46.next, new_entry_46.in4);
    
    struct xlat_entry new_entry_64 {
        .in6 = remote_6_addr;
        .in4 = remote_4_addr;
        .hash_list_data = 0; // initialised when added to hashtable
    };
    
    hash_add(xlat_64, &new_entry_64.next, new_entry_64.in6);
    
    
}


/**
 * Function to add entry to the local xlat table
 */
int local_xlat_add(in6_addr *local_6_addr, in_addr *local_4_addr){
    
    struct host_entry new_entry {
        .in6 = local_6_addr;
        .in4 = local_4_addr;
        .linked_list_data = 0; // initialised when added to linked list
    };
    
    //list_add(xlat_46, &new_entry_46.next, new_entry_46.in4);
    
    
}


/********************************
 * Lookup Functions
 ********************************/


/**
 * Function to translate local IPv6 Address to its local IPv4 address
 */
in_addr * local_64_xlat(in6_addr *local_6_addr){
    
    //for entries in ll
        //check if match
        //if yes move to head of list
        //return entry
    
    return NULL;  //No match found
}

/**
 * Function to translate local IPv6 Address to its local IPv4 address
 */
in6_addr * local_46_xlat(in_addr *local_4_addr){
    
    //for entries in ll
        //check if match
        //if yes move to head of list
        //return entry
    
    return NULL;  //No match found
}

/**
 * Function to translate remote IPv6 Address to a corresponding remote IPv4 address
 */
in_addr * remote_64_xlat(in6_addr *remote_6_addr){
    
    //hash lookup
    struct xlat_entry * current; // Pointer to current position in XLAT table
    hash_for_each_possible(xlat_64, current, hash_list_data, *remote_6_addr){
        
        // If match return pointer to corresponding IPv4 address
        if(!memcmp(current->in6,remote_6_addr)){
            //TODO Move to head
            return current->in4;
        }
    }
    
    return NULL;  //No match found
}

/**
 * Function to translate remote IPv6 Address to a corresponding remote IPv4 address
 */
in6_addr * remote_46_xlat(in_addr *remote_4_addr){
    
    //hash lookup
    struct xlat_entry * current; // Pointer to current position in XLAT table
    hash_for_each_possible(xlat_46, current, hash_list_data, *remote_4_addr){
        
        // If match return pointer to corresponding IPv4 address
        if(!memcmp(current->in4,remote_4_addr)){
            //TODO Move to head
            return current->in6;
        }
    }
    
    return NULL;  //No match found
    
    //struct in_addr *remote_4_addr;
    //inet_pton(AF_INET,"192.168.5.1",remote_4_addr);
    //return remote_4_addr;
}
