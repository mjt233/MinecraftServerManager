/********************************************************************************************************
* File name: route.h
* Description:  对接入的客户端进行请求协议识别
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: none
*********************************************************************************************************/


#ifndef SERVER_ROUTE

#endif // !SERVER_ROUTE


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
    }else if( !strcmp( buffer, "GET" ) ){
            return HTTP_GET;
    }else {
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

void * protocols_route(void * arg)
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
        case HTTP_GET:
            http_GET(IDInfo);
            while ( read( sock_fd, buffer, 1024 ) > 0 );
            close(sock_fd);
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
    shutdown( sock_fd, SHUT_RDWR );
    free(arg);
}