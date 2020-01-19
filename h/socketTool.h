#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<netinet/in.h>

int isDomainName(char * addr);
int DNtoIP(char *DN,char *ip);

int InitConnect(int *sock,char * addr,unsigned short port);
int InitServer(int *sock,char * addr,unsigned short port);

int DNtoIP(char *DN,char *ip)
{
    struct hostent *host=gethostbyname(DN);
    if(!host)
        return 0;
    else
    {
        strcpy(ip,inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));
        return 1;
    }
}

int isDomainName(char * address)
{
    int i;
    for(i=0;i<strlen(address);i++)
        if((address[i]<'0' || (address[i]>'9' ) && address[i]!='.'))
            return 1;
    return 0;
}
// Return 0 on success, -1 for errors. QWQ
int InitConnect(int *sock,char * addr,unsigned short port)
{
    char ip[20];
    *sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    struct sockaddr_in sockAddr;
    memset(&sockAddr,0,sizeof(sockAddr));
    if(isDomainName(addr))
    {
        if(!DNtoIP(addr,ip))
            return -1;
    }else
    {
        strcpy(ip,addr);
    }
    
    sockAddr.sin_family=AF_INET;
    sockAddr.sin_port=htons(port);
    sockAddr.sin_addr.s_addr=inet_addr(ip);
    return connect(*sock,(struct sockaddr*)&sockAddr,sizeof(sockAddr));
}

int InitServer(int *sock,char * addr,unsigned short port)
{
    struct sockaddr_in sockAddr;
    *sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    memset(&sockAddr,0,sizeof(sockAddr));
    sockAddr.sin_family=AF_INET;
    sockAddr.sin_port=htons(port);
    sockAddr.sin_addr.s_addr=inet_addr(addr);
    bind(*sock,(struct sockaddr*)&sockAddr,sizeof(sockAddr));
    return listen(*sock,20);
    
}