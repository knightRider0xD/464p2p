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
