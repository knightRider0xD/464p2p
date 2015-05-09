#include <linux/stddef.h>
#include <linux/sysctl.h>
#include <linux/hashtable.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/kernel.h>

/*
 * Included in 464_tables.h
 * 
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <uapi/linux/in.h>
#include <uapi/linux/in6.h>
*/

#include "464_tables.h"

/********************************
 * Init functions for XLAT table
 ********************************/

char static_table[] = "0";
char dynamic_table[] = "0";
char static_entry[56] = "0";
char dynamic_entry[56] = "0";

struct xlat_entry out_addr_cache;
struct xlat_entry in_addr_cache;

struct in_addr *in4;
struct in6_addr *in6;

char addr_4[16];
char addr_6[40];

/* two integer items (files) /
static ctl_table net_464p2p_table[] = {
   {CTL_UNNUMBERED, "static_table", &static_table,102400, 0444, NULL, &proc_dostring,NULL, /* fill with 0's *},
   {CTL_UNNUMBERED, "dynamic_table", &dynamic_table,1024000, 0444, NULL, &proc_dostring,NULL, /* fill with 0's *},
   {CTL_UNNUMBERED, "static_entry", &static_entry,56, 0644, NULL, &proc_dostring,NULL, /* fill with 0's *},
   {CTL_UNNUMBERED, "dynamic_entry", &dynamic_entry,56, 0644, NULL, &proc_dostring,NULL, /* fill with 0's *},
   {0}
   };

/* a directory *
static ctl_table net_table[] = {
        {CTL_UNNUMBERED, "464p2p", NULL, 0, 0555,
            net_464p2p_table},
        {0}
    };

/* the parent directory *
static ctl_table root_table[] = {
        {CTL_NET, "net", NULL, 0, 0555,
            net_table},
        {0}
    }; 

static struct ctl_table_header *ctl_table_header;
*/

int init_tables()
{
    /*ctl_table_header = register_sysctl_table(root_table,0);
    //return ctl_table_header;*/
   
   
    DEFINE_HASHTABLE(xlat_46, 16);
    DEFINE_HASHTABLE(xlat_64, 16);

    LIST_HEAD(xlat_local);

    return 0;
    
}

int cleanup_tables()
{
    /*if(ctl_table_header!=NULL){
        unregister_sysctl_table(ctl_table_header);
    }*/
    return 0;
}

/********************************
 * XLAT Tables
 ********************************/

/**
 * Function to parse XLAT Entry from string
 */
int xlat_add(char xlat_str[]){
    
    strncpy(addr_6,xlat_str,39);
    
    in6 = kzalloc(sizeof(struct in6_addr),GFP_KERNEL);
    
    if(in6_pton(addr_6,in6)!=0){
        kfree(in6);
        return -1;
    }
    
    
    strncpy(addr_4,xlat_str+40,15);
    
    in4 = kzalloc(sizeof(struct in_addr),GFP_KERNEL);
    
    if(in4_pton(addr_4,in4)!=0){
        kfree(in6);
        kfree(in4);
        return -2;
    }
    
    remote_xlat_add(in6, in4);
    return 0;
    
}

/**
 * Function to parse static XLAT Entry
 */
int static_xlat_add(){
    
    strncpy(static_entry,"0000:0000:0000:0000:0000:0000:0000:0000 000.000.000.000",56);
    xlat_add(static_entry);
    return 1;
    /*
    //Check entry if done, next or new
    if (!strncmp(static_entry,"0",2)){
        //Already done
        return 0;
    } else if (!strncmp(static_entry,"1",2)){
        //No more
        return 1;
    }
    
    //New entry to process
    int res = xlat_add(static_entry);
    if(res>=0){
        //Processed entry okay? Yes; acknowledge via sysctl for userspace interface to continue.
        strncpy(static_entry,"0",2);
    }
    return res;
    */
    
}

/**
 * Function to parse dynamic XLAT Entry
 */
int dynamic_xlat_add(){
    
    return 0;
    
    //Not yet implemented
    //return xlat_add(dynamic_entry);
    
    
}


/**
 * Function to add entry to the remote xlat table
 */
int remote_xlat_add(struct in6_addr *remote_6_addr, struct in_addr *remote_4_addr){
    
    struct xlat_entry new_entry_46 = {
        .in6 = remote_6_addr,
        .in4 = remote_4_addr,
        .hash_list_data = 0 // initialised when added to hashtable
    };
    
    hash_add(xlat_46, &new_entry_46.next, new_entry_46.in4);
    
    struct xlat_entry new_entry_64 = {
        .in6 = remote_6_addr,
        .in4 = remote_4_add,
        .hash_list_data = 0 // initialised when added to hashtable
    };
    
    hash_add(xlat_64, &new_entry_64.next, new_entry_64.in6);
    
    return 0;
    
}


/**
 * Function to add entry to the local xlat table
 */
int local_xlat_add(struct in6_addr *local_6_addr, struct in_addr *local_4_addr){
    
    struct host_entry new_entry = {
        .in6 = local_6_addr,
        .in4 = local_4_addr,
        .linked_list_data = 0 // initialised when added to linked list
    };
    
    list_add(&new_entry,xlat_local);
    
    return 0;
    
}


/********************************
 * Lookup Functions
 ********************************/


/**
 * Function to translate local IPv6 Address to its local IPv4 address
 */
struct in_addr * local_64_xlat(struct in6_addr *local_6_addr){
    
    //list lookup
    struct host_entry * current; // Pointer to current position in XLAT list
    list_for_each(current, xlat_local){
        
        // If match return pointer to corresponding IPv4 address
        if(!memcmp(current->in6,local_6_addr)){
            //TODO Move to head
            return current->in4;
        }
    }
    
    return NULL;  //No match found
}

/**
 * Function to translate local IPv6 Address to its local IPv4 address
 */
struct in6_addr * local_46_xlat(struct in_addr *local_4_addr){
    
    //list lookup
    struct host_entry * current; // Pointer to current position in XLAT list
    list_for_each(current, xlat_local){
        
        // If match return pointer to corresponding IPv4 address
        if(!memcmp(current->in4,local_4_addr)){
            //TODO Move to head
            return current->in6;
        }
    }
    
    return NULL;  //No match found
}

/**
 * Function to translate remote IPv6 Address to a corresponding remote IPv4 address
 */
struct in_addr * remote_64_xlat(struct in6_addr *remote_6_addr){
    
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
struct in6_addr * remote_46_xlat(struct in_addr *remote_4_addr){
    
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
    //in4_pton("192.168.5.1",remote_4_addr);
    //return remote_4_addr;
}


/********************************
 * pton Functions
 ********************************/


/**
 * Function to convert dotted decimal IPv4 address string to in_addr structure
 */
int in4_pton(char *str, struct in_addr *target_addr){
    
    //Test for proper addr len
    if(strlen(str))!=15){
        return 1;  //Unable to convert
    }
    
    // setup buffers
    char buf[4];
    long lval = 0;
    uint32_t wrk = 0;
    
    // convert octet a
    strncpy(buf,&str[0],3); //extract octet chars from string
    kstrtol(buf,10,&lval); // convert to long
    if (lval<0){ // if did not convert, set octet to 0
        lval=0;
    }
    wrk += (uint32_t) lval; // add to working addr
    wrk <<= 8; // shift octet over to make room for next one
    
    // convert octet b
    strncpy(buf,&str[4],3);
    kstrtol(buf,10,&lval);
    if (lval<0){
        lval=0;
    }
    wrk += (uint32_t) lval;
    wrk <<= 8;
    
    // convert octet c
    strncpy(buf,&str[8],3);
    kstrtol(buf,10,&lval);
    if (lval<0){
        lval=0;
    }
    wrk += (uint32_t) lval;
    wrk <<= 8;
    
    // convert octet d
    strncpy(buf,&str[12],3);
    kstrtol(buf,10,&lval);
    if (lval<0){
        lval=0;
    }
    wrk += (uint32_t) lval;
    
    target_addr->s_addr = wrk;
    return 0;
    
}

/**
 * Function to convert colon-separated hexadecimal IPv6 address string to in6_addr structure
 */
int in6_pton(char *str, struct in6_addr *target_addr){
    
    //Test for proper addr len
    if(strlen(str))!=39){
        return 1;  //Unable to convert
    }
    
    // setup buffers
    char buf[3];
    long lval = 0;
    
    int i=0;
    for(i=0; i<8; i++){ // for each 16 bit group
        
        //top 2 bytes of group
        strncpy(buf,&str[0+(5*i)],2); // extract hex chars from str
        kstrtol(buf,16,&lval); // convert to long
        if (lval<0){ // if did not convert, set byte to 0
            lval=0;
        }
        target_addr->s6_addr[(2*i)] = (char) lval; // Copy byte to dest struct.

        // repeat for bottom 2 bytes of group
        strncpy(buf,&str[2+(5*i)],2);
        kstrtol(buf,16,&lval);
        if (lval<0){
            lval=0;
        }
        target_addr->s6_addr[(2*i)+1] = (char) lval;

    }    

    return 0;
}