/********************************************************************************************************
* File name: socketPipe.h
* Description:  利用socket作为管道使用
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.31
* History: none
*********************************************************************************************************/

#include<string.h>
#include<unistd.h>
#ifdef WIN32
#include<winsock2.h>
#include<windows.h>
#pragma comment (lib, "ws2_32.lib")
#define SOCKET_T SOCKET
#endif // WIN32

#ifdef linux
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<netinet/in.h>
#define SOCKET_T int
#endif // linux
#include<cstdlib>
#include<ctime>
#include<thread>
class socketPipe{
    private:
        SOCKET_T SerSocket;
        SOCKET_T AcceptSocket;
        unsigned short port;
        int Connect(SOCKET_T *sock,unsigned short port);
        int Listen(SOCKET_T *sock,unsigned short port);
        void DataRedirect();
        void Accept();
        std::thread *redTh;
    public:
        SOCKET_T psocket;
        void write(const char * buf, size_t t);
        socketPipe()
        {
            int success = 0;
            int t;
            time_t timer;
            srand(time(&timer));
            while ( success!=1 )
            {
                port = rand()%64535+1000;
                if( Connect(&t,port) == 0 )
                {
                    close(t);
                    continue;
                }
                success++;
            }
            Listen(&SerSocket, port);
            std::thread acceptTh(&socketPipe::Accept,this);
            Connect(&psocket, port);
            acceptTh.join();
            redTh = new std::thread(&socketPipe::DataRedirect,this);
        }
        ~socketPipe()
        {
            close(SerSocket);
            close(AcceptSocket);
            redTh->detach();
            delete(redTh);
        }
};

void socketPipe::write(const char * buf, size_t t)
{
    size_t count = 0,total = 0,tmp;
    while ( total < t )
    {
        tmp = t - total;
        count = send(psocket, buf, tmp > 2048?2048:tmp, 0);
        total += count;
    }
    // usleep(25000);
}

void socketPipe::DataRedirect()
{
    char buffer[8192];
    int count;
    while ( ( count = recv(AcceptSocket, buffer, 8192, 0) ) > 0 )
    {
        send(AcceptSocket, buffer, count, MSG_WAITALL);
    }
    
}

void socketPipe::Accept()
{
    struct sockaddr_in sockaddr;
    socklen_t len = sizeof(sockaddr);
    AcceptSocket = accept(SerSocket, (struct sockaddr *)&sockaddr, &len);
}


int socketPipe::Connect(SOCKET_T *sock,unsigned short port)
{
    *sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    struct sockaddr_in sockAddr;
    memset(&sockAddr,0,sizeof(sockAddr));
    sockAddr.sin_family=AF_INET;
    sockAddr.sin_port=htons(port);
    sockAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    return connect(*sock,(struct sockaddr*)&sockAddr,sizeof(sockAddr));
}

int socketPipe::Listen(SOCKET_T *sock,unsigned short port)
{
    struct sockaddr_in sockAddr;
    *sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    memset(&sockAddr,0,sizeof(sockAddr));
    sockAddr.sin_family=AF_INET;
    sockAddr.sin_port=htons(port);
    sockAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    int reuse = 1;
    setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuse, sizeof(int));
    bind(*sock,(struct sockaddr*)&sockAddr,sizeof(sockAddr));
    return listen(*sock,20);
    
}