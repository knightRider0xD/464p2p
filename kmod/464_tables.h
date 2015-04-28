#include <linux/ip.h>
#include <linux/ipv6.h>
#include <uapi/linux/in.h>
#include <uapi/linux/in6.h>

struct xlat_entry {
     in6_addr *in6;
     in_addr *in4;
     struct hlist_node hash_list_data;
};

struct host_entry {
     in6_addr *in6;
     in_addr *in4;
     struct list_head linked_list_data;
};

int init_tables();
int cleanup_tables();

int static_xlat_add();
int dynamic_xlat_add();
int local_xlat_add(in6_addr *local_6_addr, in_addr *local_4_addr);

in_addr * local_64_xlat(in6_addr *local_6_addr);
in6_addr * local_46_xlat(in_addr *local_4_addr);
in_addr * remote_64_xlat(in6_addr *remote_6_addr);
in6_addr * remote_46_xlat(in_addr *remote_4_addr);

int in4_pton(char *str, in_addr *target_addr);
int in6_pton(char *str, in6_addr *target_addr);