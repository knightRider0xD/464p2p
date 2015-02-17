#include <linux/sysctl.h>

#define NET_464P2P 464 /* random number */
#define STATIC_TABLE_ID 1
#define DYNAMIC_TABLE_ID 2

int busy_ontime = 0;   /* loop 0 ticks */
int busy_offtime = HZ; /* every second */

/* two integer items (files) */
static ctl_table net_464p2p_table[] = {
   {STATIC_TABLE_ID, "static_table", &busy_ontime,
   sizeof(int), 0644, NULL, &proc_dointvec,
   _intvec, /* fill with 0's */},
   {DYNAMIC_TABLE_ID, "dynamic_table", &busy_offtime,
   sizeof(int), 0644, NULL, &proc_dointvec,
   _intvec, /* fill with 0's */},
   {0}
   };

/* a directory */
static ctl_table net_table[] = {
        {NET_464P2P, "464p2p", NULL, 0, 0555,
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
}

void cleanup_module(void)
{
        unregister_sysctl_table(ctl_table_header);
}