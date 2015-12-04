#ifndef __464_NETLINK
#define __464_NETLINK

#ifndef NETLINK_464P2P
#define NETLINK_464P2P           24
#endif /* NETLINK_464P2P */

#define NL464_ACK               0x80000000
#define NL464_DATA4             0x40000000
#define NL464_DATA6             0x20000000

//#define NL464_LOCAL_STATUS      0x00008000
#define NL464_LOCAL_ADD         0x00004000
//#define NL464_LOCAL_REMOVE      0x00002000

//#define NL464_REMOTE_STATUS     0x00000080
#define NL464_REMOTE_ADD        0x00000040
//#define NL464_REMOTE_REMOVE     0x00000020

int init_netlink(void);
int cleanup_netlink(void);

#endif /* __464_NETLINK */