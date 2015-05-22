#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

int main(int argc, char**argv)
{

    int sockfd, res;
    struct addrinfo* addr_dets=0;
    struct addrinfo addr_hints;
    

    if (argc != 4)
    {
        printf("Usage:  udp-send4 <Dest IPv4 address> <Dest Portnum> <Message>\n");
        exit(1);
    }
    
    //Setup addr info hints
    bzero(&addr_hints,sizeof(addr_hints));
    addr_hints.ai_family=AF_UNSPEC;
    addr_hints.ai_socktype=SOCK_DGRAM;
    addr_hints.ai_protocol=IPPROTO_UDP;
    addr_hints.ai_flags=AI_NUMERICHOST|AI_NUMERICSERV;
    addr_hints.ai_canonname = NULL;
    addr_hints.ai_addr = NULL;
    addr_hints.ai_next = NULL;
    
    
    if (getaddrinfo(argv[1],argv[2],&addr_hints,&addr_dets)!=0) {
        printf("Invalid Dest Address/Portnum. Exiting.\n");
        exit(2);
    }

    //Establish socket
    if (sockfd=socket(addr_dets->ai_family,addr_dets->ai_socktype,addr_dets->ai_protocol) < 0){
        printf("Couldn't open socket. Exiting.\n");
        exit(3);
    }
    
    
    //Send datagram
    res = sendto(sockfd,argv[3],sizeof(argv[3])+1,0,addr_dets->ai_addr,addr_dets->ai_addrlen);
    
    if (res < 0){
        printf("Error sending datagram:\nRET:%d\nExiting.\n",res);
        exit(4);
    }
    
    printf("%d Bytes Sent Successfully. Finished.\n",res);
    exit(0);
} 
