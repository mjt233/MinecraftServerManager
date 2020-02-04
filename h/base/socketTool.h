/********************************************************************************************************
* File name: socketTool.h
* Description:  简单的socket库封装
* Author: mjt233@qq.com
* Version: 2.0
* Date: 2019.2.4
* History: 
    * 2019-2-1 增加Windows支持
    * 2019-2-4 增加可设置超时的connect
*********************************************************************************************************/
#ifndef WIN32
    #ifndef linux
    #define linux
    #endif // !linux
#endif // !WIN32

#include<string.h>
#include<unistd.h>
#ifdef WIN32
#include<winsock2.h>
#include<windows.h>
#pragma comment (lib, "ws2_32.lib")
#define SOCKET_T SOCKET
int PASCAL FAR setsockopt( SOCKET s, int level, int optname,const void FAR *optval, int optlen);
#endif // WIN32

#ifdef linux
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<netinet/in.h>
#include <fcntl.h>
#define SOCKET_T int
#endif // linux


int isDomainName(const char * addr);
int DNtoIP(const char *DN,char *ip);

int InitConnect(SOCKET_T *sock,const char * addr,unsigned short port);
int InitServer(SOCKET_T *sock,char * addr,unsigned short port);
int ConnectTimeOut(SOCKET_T *sock,const char * addr, unsigned short port, int timeout);
void InitSocket()
{
    #ifdef WIN32
    WSADATA wsaData;
    WSAStartup( MAKEWORD(2,2), &wsaData );
    #endif // WIN32
}

// Return 1 on success, -1 or -2 for errors. QWQ
int ConnectTimeOut(SOCKET_T *sock,const char * addr, unsigned short port, int timeout)
{
    char ip[20];
    *sock = 0;
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
    #ifdef linux
    fcntl(*sock, F_SETFL, fcntl(*sock, F_GETFL, 0) | O_NONBLOCK);
    #endif // linux
    int ret = connect(*sock,(struct sockaddr*)&sockAddr,sizeof(sockAddr));
    if( ret != 0 )
    {
        if(errno != EINPROGRESS)
        {
            return -1;
        }else
        {
            struct timeval tm = {timeout,0};
            fd_set wset,rset;
            FD_ZERO(&wset);
            FD_ZERO(&rset);
            FD_SET(*sock, &wset);
            FD_SET(*sock, &rset);
            int res = select(*sock + 1, &rset, &wset, NULL, &tm);
            if( res < 0 )
            {
                return -1;
            }else if( res == 0 )
            {
                return -2;
            }else if( res == 1 )
            {
                if( FD_ISSET(*sock, &wset) )
                {
                    fcntl(*sock, F_SETFL, fcntl(*sock, F_GETFL, 0) & ~O_NONBLOCK);
                    return 1;
                }else
                {
                    return 0;
                }
            }else
            {
                return 0;
            }
        }
    }else
    {
        return 0;
    }
    
}


int DNtoIP(const char *DN,char *ip)
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

int isDomainName(const char * address)
{
    int i;
    for(i=0;i<strlen(address);i++)
        if((address[i]<'0' || (address[i]>'9' ) && address[i]!='.'))
            return 1;
    return 0;
}
// Return 0 on success, -1 for errors. QWQ
int InitConnect(SOCKET_T *sock,const char * addr,unsigned short port)
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

int InitServer(SOCKET_T *sock,char * addr,unsigned short port)
{
    struct sockaddr_in sockAddr;
    *sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    memset(&sockAddr,0,sizeof(sockAddr));
    sockAddr.sin_family=AF_INET;
    sockAddr.sin_port=htons(port);
    sockAddr.sin_addr.s_addr=inet_addr(addr);

    int reuse = 1;

    struct linger lg;
    lg.l_linger = 5;
    lg.l_onoff = 0;

    setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(int));
    setsockopt(*sock, SOL_SOCKET, SO_LINGER, (char*)&lg, sizeof(lg));
    bind(*sock,(struct sockaddr*)&sockAddr,sizeof(sockAddr));
    return listen(*sock,20);
    
}