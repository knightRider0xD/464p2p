#ifndef __464_NETLINK
#define __464_NETLINK

#ifndef NETLINK_464P2P
#define NETLINK_464P2P           46
#endif /* NETLINK_464P2P */

#define NL464_ACK               0x80000000
#define NL464_DATA4             0x40000000
#define NL464_DATA6             0x20000000

//#define NL464_LOCAL_STATUS      0x8000
#define NL464_LOCAL_ADD         0x4000
//#define NL464_LOCAL_REMOVE      0x2000

//#define NL464_REMOTE_STATUS     0x0080
#define NL464_REMOTE_ADD        0x0040
//#define NL464_REMOTE_REMOVE     0x0020

int init_netlink(void);
int cleanup_netlink(void);

#endif /* __464_NETLINK */