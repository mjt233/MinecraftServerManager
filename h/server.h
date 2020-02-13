/********************************************************************************************************
* File name: server.h
* Description:  基础服务 如监听端口，处理接入的连接
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: none
*********************************************************************************************************/


#include "protocols/route.h"

void * manager_server_start(void * arg)
{
    
    // 监听端口
    InitServer(&SER_SOCKET,( char * )"0.0.0.0",6636);
    cout << "\r端口开始监听" << endl;
    fflush(stdout);
    int client_socket;
    struct sockaddr_in cli_addr;

    int count = 0;
    while (1)
    {
        int *client_socket;
        pthread_t thid;
        client_socket =  new int;
        socklen_t socklen = sizeof( cli_addr );
        if ( ( *client_socket = accept( SER_SOCKET, (struct sockaddr*)&cli_addr, &socklen ) ) ==-1 )
        {
            cout<< "\rServer Exit.... " << endl;
            cout << errno << endl;
            break;
        }

        if(DEBUG_MODE)
        {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET,&cli_addr.sin_addr.s_addr,ip,sizeof(ip));
            // cout << "ip : " << ip << endl;
        }

        // 开启线程取处理
        pthread_create( &thid, NULL, protocols_route , (void*)client_socket );
    }
}
