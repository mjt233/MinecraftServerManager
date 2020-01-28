/********************************************************************************************************
* File name: ctl.h
* Description:  接入的控制器功能声明与定义
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: none
*********************************************************************************************************/

void * controller_read_pipe(void * Controller_arg);                 // 从Controller中的pipe读取数据并发送到socket
void * controller_read_socket(void * Controller_arg);               // 从Controller中的socket读取数据并发送到所接入的Server的socket
void controller_join(baseInfo &IDInfo);                             // 控制器接入

void controller_join(baseInfo &IDInfo)
{
    Controller *ctl = new Controller(IDInfo.SerID, IDInfo.UsrID, IDInfo.socket);
    cout << "Controller Socket: " << IDInfo.socket << endl;
    pthread_join(ctl->thid, NULL);
    pthread_join(ctl->thid2, NULL);
    delete( ctl );
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
        memset(buffer, 0, sizeof(buffer));
    }
    if( !ctl->isClose )
    {
        ctl->stop();
    }
    
}

