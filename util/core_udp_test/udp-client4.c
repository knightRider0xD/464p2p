#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

int main(int argc, char**argv)
{

    int sockfd, res;
    struct sockaddr_in dst_addr;

    if (argc != 4)
    {
        printf("Usage:  udp-send4 <Dest IPv4 address> <Dest Portnum> <Message>\n");
        exit(1);
    }

    //Establish socket
    sockfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (socket < 0){
        printf("Couldn't open socket. Exiting.\n");
        exit(2);
    }

    //Zero out dst_addr ready for data
    bzero(&dst_addr,sizeof(dst_addr));

    //Populate dst_addr
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port=htons(atoi(argv[2]));
    if(inet_pton(AF_INET, argv[1], &dst_addr.sin_addr) < 0){
        printf("Invalid Dest Address. Exiting.\n");
        exit(3);
    }
    
    //Send datagram
    res = sendto(sockfd,argv[3],strlen(argv[3])+1,0,(struct sockaddr *)&dst_addr,sizeof(dst_addr));
    
    if (res < 0){
        printf("Error sending datagram %d. Exiting.\n",res);
        exit(4);
    }
    
    printf("%d Bytes Sent Successfully. Finished.\n",res);
    exit(0);
}