/********************************************************************************************************
* File name: ctl.h
* Description:  接入的服务器功能声明与定义
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: none
*********************************************************************************************************/
void * server_write_ctl_pipe(void * arg);
void server_join(baseInfo &IDInfo);                                 // 服务器接入

void server_join(baseInfo &IDInfo)
{

    Server *ser = new Server(IDInfo.SerID, IDInfo.UsrID, IDInfo.socket);
    write( ser->socket, "OK", 2 );
    // 阻塞 等待服务器读取线程结束
    ser->th1->join();
    // 释放内存
    delete( ser );

}

void Server::server_read()
{
    char buffer[DEFAULT_CHAR_BUFFER_SIZE];
    while ( read( this->socket, buffer, 1024 ) > 0 )
    {
        // 向该Server的缓冲字符串追加写入数据
        this->sbMutex.lock();
        buffer_append(&this->sb, buffer);
        this->sbMutex.unlock();

        // 广播到连接到该服务器的所有控制器
        this->Broadcast(buffer, strlen(buffer));
        memset(buffer, 0,sizeof(buffer));
    }
}

void Server::server_write()
{
    Server *ser = this;
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
    CTLMessage *a = (CTLMessage*)arg;
    write( a->ctl->socket, a->msg.c_str(), a->msg.length() );
    free(arg);
}

void Server::addController(Controller *ctl)
{
    ctlMutex.lock();
    CTLList.push_back(ctl);
    ctlMutex.unlock();
    sbMutex.lock();
    char * strbuf = buffer_get_string(&sb);
    sbMutex.unlock();
    if ( !strbuf )
    {
        cout << "malloc error" << endl;
    }
    size_t count = 0 , len = strlen(strbuf),t;
    while ( count < len )
    {
        t = write( ctl->socket, strbuf+count, len - count );
        if( t<=0 )
        {
            break;
        }
        count += t;
    }
    free(strbuf);
    
}