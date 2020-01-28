/********************************************************************************************************
* File name: ctl.h
* Description:  接入的服务器功能声明与定义
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: none
*********************************************************************************************************/

void * server_read(void * arg);                                     // 从Server的socket读取数据并处理
void * server_write(void * arg);                                    // 往Server的socket写入数据
void * server_write_ctl_pipe(void * arg);                           // 往Server的Controller的pipe_fd[1]写入数据
void server_join(baseInfo &IDInfo);                                 // 服务器接入


void server_join(baseInfo &IDInfo)
{

    Server *ser = new Server(IDInfo.SerID, IDInfo.UsrID, IDInfo.socket);
    write( ser->socket, "OK", 2 );
    // 阻塞 等待服务器读取线程结束
    pthread_join( ser->thid, NULL );
    // 释放内存
    delete( ser );

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
