#ifndef __464_TABLES
#define __464_TABLES

#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/in.h>
#include <linux/in6.h>

struct xlat_entry {
     struct in_addr *in4;
     struct in6_addr *in6;
     struct hlist_node hash_list_data;
};

struct host_entry {
     struct in_addr *in4;
     struct in6_addr *in6;
     struct list_head linked_list_data;
};

int init_tables(void);
int cleanup_tables(void);

int local_xlat_add(struct in6_addr *local_6_addr, struct in_addr *local_4_addr);
int remote_xlat_add(struct in6_addr *remote_6_addr, struct in_addr *remote_4_addr);

struct in_addr * local_64_xlat(struct in6_addr *local_6_addr);
struct in6_addr * local_46_xlat(struct in_addr *local_4_addr);
struct in_addr * remote_64_xlat(struct in6_addr *remote_6_addr);
struct in6_addr * remote_46_xlat(struct in_addr *remote_4_addr);

#endif /* __464_TABLES */