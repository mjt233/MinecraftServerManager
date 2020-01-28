#include"h/protocols/route.h"
#include"h/protocols/access/ctl.h"
#include"h/protocols/access/ser.h"

void * manager_server_start(void * arg)
{
    
    // 监听端口
    InitServer(&SER_SOCKET,( char * )"127.0.0.1",6636);
    cout << "\r端口开始监听" << endl << "$ ";
    fflush(stdout);
    // 初始化锁
    memset(&SerMutex, 0, sizeof(SerMutex));
    pthread_mutex_init(&SerMutex, NULL);

    int client_socket;
    struct sockaddr_in cli_addr;
    socklen_t socklen = sizeof( cli_addr );

    int count = 0;
    while (1)
    {
        int *client_socket;
        pthread_t thid;
        client_socket =  new int;
        if ( ( *client_socket = accept( SER_SOCKET, (struct sockaddr*)&cli_addr, &socklen ) ) ==-1 )
        {
            cout<< "\rServer Exit.... " << endl << "$ ";
            break;
        }
        // 开启线程取处理
        pthread_create( &thid, NULL, protocols_route , (void*)client_socket );
    }
}
