/********************************************************************************************************
* File name: route.h
* Description:  对接入的客户端进行请求协议识别
* Author: mjt233@qq.com
* Version: 1.1
* Date: 2019.2.6
* History: 
*   2019.2.6    增加HTTP POST识别支持
*   2019.1.28   创建
*********************************************************************************************************/


#include "access/ctl.h"
#include "access/ser.h"
#include "access/transferfile.h"
#include "http.h"
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
    recv( sock_fd, buffer,3, MSG_WAITALL);
    // 获取请求接入的类型
    if ( !strcmp( buffer, "SER" ) )
    {
        tag = SERVER;
    }else if( !strcmp( buffer, "CTL" ) )
    {
        tag = CONTROLLER;
    }else if( !strcmp( buffer, "TSF" ) )
    {
        tag = TRANSFERFILE;
    }else if( !strcmp( buffer, "GET" ) ){
            return HTTP_GET;
    }else if( !strcmp( buffer, "POS") ) {
        return HTTP_POST;
    }else{
        return -1;
    }

    // 获取请求的服务器ID
    if ( recv(sock_fd, buffer, 6, MSG_WAITALL) <= 0 || buffer[5]!='E' || ( IDInfo.SerID = atoi( buffer ) ) <= 0 )
    {
        return SERID_FORMAT_INCOREET;
    }

    // 判断服务器ID是否冲突
    SerMutex.lock();
    isServerExist = SerList.count( IDInfo.SerID );
    SerMutex.unlock();
    if ( tag == SERVER && isServerExist )
    {
        return SERID_EXIST;
    }else if( (tag == CONTROLLER || tag == TRANSFERFILE) && !isServerExist )
    {
        return SERID_UNEXIST;
    }

    if ( recv(sock_fd, buffer, 6, MSG_WAITALL) <= 0 || buffer[5]!='E' || ( IDInfo.UsrID = atoi( buffer ) ) <= 0 )
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
            break;
        case CONTROLLER:
            controller_join(IDInfo);
            break;
        case TRANSFERFILE:
            transferfile(IDInfo);
            break;
        case HTTP_GET:
            http_GET(IDInfo);
            while ( recv( sock_fd, buffer, 1024, 0 ) > 0 );
            close(sock_fd);
            break;
        case HTTP_POST:
            http_post(IDInfo);
            break;
        case SERID_EXIST:
            DEBUG_OUT("服务器接入请求被拒绝,服务器ID已存在");
            write(sock_fd, "SERID_EXIST",12);
            close(sock_fd);
            break;
        case SERID_UNEXIST:
            DEBUG_OUT("控制器接入请求被拒绝,服务器ID不存在");
            write(sock_fd, "SERID_UNEXIST",14);
            close(sock_fd);
            break;
        case SERID_FORMAT_INCOREET:
            DEBUG_OUT("请求被拒绝,服务器ID格式不正确");
            write(sock_fd, "SERID_FORMAT_INCOREET",22);
            close(sock_fd);
            break;
        case USRID_FORMAT_INCOREET:
            DEBUG_OUT("请求被拒绝,用户ID不存在");
            write(sock_fd, "USRID_FORMAT_INCOREET",22);
            close(sock_fd);
            break;
        default:
            break;
    }
    shutdown( sock_fd, SHUT_RDWR );
    free(arg);
}