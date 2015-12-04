#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

int main(int argc, char**argv)
{

    int sockfd, res, len;
    struct sockaddr_in src_addr,dst_addr;
    char mesg[1000];

    if (argc != 2)
    {
        printf("Usage:  server4 <Listen Portnum>\n");
        exit(1);
    }

    //Establish socket
    sockfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (sockfd < 0){
        printf("Couldn't open socket. Exiting.\n");
        exit(2);
    }
    
    bzero(&dst_addr,sizeof(dst_addr));
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    dst_addr.sin_port=htons(atoi(argv[1]));
    bind(sockfd,(struct sockaddr *)&dst_addr,sizeof(dst_addr));

    while(1){
        //Zero out src_addr ready for data
        len = sizeof(src_addr);
        bzero(&src_addr,len);
        
        //Send datagram
        res = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&src_addr,&len);
        
        if (res < 0){
            printf("Error sending datagram %d. Exiting.\n",res);
            exit(4);
        }
        
        mesg[res]=0;
        
        printf("%d Bytes Revcieved Successfully.\nMESSAGE:%s\nFinished.\n",res,mesg);
    }
    
    exit(0);
}