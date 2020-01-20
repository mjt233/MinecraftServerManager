class Client;
class Controller;
class Server;

int read_request_id( int sock_fd, baseInfo &IDInfo )
{
    char buffer[DEFAULT_CHAR_BUFFER_SIZE];
    int tag=-1,
        UsrID = 0,
        SerID = 0,
        isServerExist = 0;
    IDInfo.socket = sock_fd;
    read( sock_fd, buffer,3);
    // 获取请求接入的类型
    if ( !strcmp( buffer, "SER" ) )
    {
        tag = SERVER;
    }else if( !strcmp( buffer, "CTL" ) )
    {
        tag = CONTROLLER;
    }else if( !strcmp( buffer, "CMD" ) )
    {
        tag = COMMAND;
    }else{
        return -1;
    }

    // 获取请求的服务器ID
    if ( read(sock_fd, buffer, 6) <= 0 || buffer[5]!='E' || ( IDInfo.SerID = atoi( buffer ) ) <= 0 )
    {
        return SERID_FORMAT_INCOREET;
    }

    // 判断服务器ID是否冲突
    pthread_mutex_lock(&SerMutex);
    isServerExist = SerList.count( IDInfo.SerID );
    pthread_mutex_unlock(&SerMutex);
    if ( tag == SERVER && isServerExist )
    {
        return SERID_EXIST;
    }else if( (tag == CONTROLLER || tag == COMMAND) && !isServerExist )
    {
        return SERID_UNEXIST;
    }

    if ( read(sock_fd, buffer, 6) <= 0 || buffer[5]!='E' || ( IDInfo.UsrID = atoi( buffer ) ) <= 0 )
    {
        return USRID_FORMAT_INCOREET;
    }
    return tag;
    
}

void server_join(baseInfo &IDInfo)
{

    Server *ser = new Server(IDInfo.SerID, IDInfo.UsrID, IDInfo.socket);
    write( ser->socket, "OK", 2 );
    // 阻塞 等待服务器读取线程结束
    pthread_join( ser->thid, NULL );
    // 释放内存
    delete( ser );

}

void controller_join(baseInfo &IDInfo)
{
    Controller *ctl = new Controller(IDInfo.SerID, IDInfo.UsrID, IDInfo.socket);
    cout << "Controller Socket: " << IDInfo.socket << endl;
    pthread_join(ctl->thid, NULL);
    pthread_join(ctl->thid2, NULL);
    delete( ctl );
}



void * entrance(void * arg)
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
    while (1)
    {
        int *client_socket;
        pthread_t thid;
        client_socket =  new int;
        if ( ( *client_socket = accept( SER_SOCKET, (struct sockaddr*)&cli_addr, &socklen ) ) ==-1 )
        {
            cout<< "\r connect accept Error! " << endl << "$ ";
            break;
        }
        // 开启线程取处理
        pthread_create( &thid, NULL, process_connect , (void*)client_socket );
    }
}


void * process_connect(void * arg)
{

    int sock_fd = *(int *)arg;
    char buffer[DEFAULT_CHAR_BUFFER_SIZE];
    baseInfo IDInfo;
    int tag;
    tag = read_request_id( sock_fd, IDInfo );

    switch (tag)
    {
        case SERVER:
            server_join(IDInfo);
            cout << "\r服务器退出" << endl << "$ ";
            fflush(stdout);
            break;
        case CONTROLLER:
            controller_join(IDInfo);
            cout << "\r控制器退出" << endl << "$ ";
            fflush(stdout);
            break;
        case COMMAND:
            break;
        case SERID_EXIST:
            write(sock_fd, "SERID_EXIST",12);
            close(sock_fd);
            break;
        case SERID_UNEXIST:
            write(sock_fd, "SERID_UNEXIST",14);
            close(sock_fd);
            break;
        case SERID_FORMAT_INCOREET:
            write(sock_fd, "SERID_FORMAT_INCOREET",22);
            close(sock_fd);
            break;
        case USRID_FORMAT_INCOREET:
            write(sock_fd, "USRID_FORMAT_INCOREET",22);
            close(sock_fd);
            break;
        default:
            break;
    }
    close(sock_fd);
    free(arg);
}

void * server_read(void * arg)
{
    Server *ser = (Server*)arg;
    char buffer[DEFAULT_CHAR_BUFFER_SIZE];
    while ( read( ser->socket, buffer, 1024 ) > 0 )
    {
        // 向该Server的缓冲字符串追加写入数据
        pthread_mutex_lock(&ser->sbMutex);
        buffer_append(&ser->sb, buffer);
        pthread_mutex_unlock(&ser->sbMutex);

        // 广播到连接到该服务器的所有控制器
        ser->Broadcast(buffer, strlen(buffer));
        memset(buffer, 0,sizeof(buffer));
    }
    return nullptr;
}

void * server_write(void * arg)
{
    Server *ser = (Server*)arg;
    char buffer[DEFAULT_CHAR_BUFFER_SIZE];

    // 从Server内的管道读取数据,将数据转发给socket文件描述符 
    // 数据来源:在server_read线程中,Server读取socket文件描述符后写入内部管道
    while( read(ser->pipe_fd[0], buffer, strlen(buffer) ) > 0 )
    {
        write(ser->socket, buffer, strlen(buffer));
    }

}


void * server_write_ctl_pipe(void * arg)
{
    CTLMessage *CTLM = (CTLMessage *)arg;
    write( CTLM->ctl->pipe_fd[1], CTLM->buffer, strlen(CTLM->buffer) );

    // 释放传参结构体占用的内存
    delete(CTLM);
}

void * controller_read_pipe(void * Controller_arg)
{
    Controller *ctl = (Controller*)Controller_arg;
    char buffer[DEFAULT_CHAR_BUFFER_SIZE];
    while ( read( ctl->pipe_fd[0], buffer, 1024 ) > 0)
    {
        if( ctl->isClose )
        {
            break;
        }
        write( ctl->socket, buffer, strlen(buffer) );
        memset(buffer, 0, sizeof(buffer));
    }
}

void * controller_read_socket(void * Controller_arg)
{
    Controller *ctl = (Controller*)Controller_arg;
    char buffer[DEFAULT_CHAR_BUFFER_SIZE];
    while ( read(ctl->socket, buffer, 1024) > 0 )
    {
        if( ctl->isClose )
        {
            break;
        }
        write( ctl->ser->socket, buffer, strlen(buffer) );
    }
    if( !ctl->isClose )
    {
        ctl->stop();
    }
    
}

