/********************************************************************************************************
* File name: ctl.h
* Description:  接入的控制器功能声明与定义
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: none
*********************************************************************************************************/
void invert(char * buf, size_t len);
void * controller_read_pipe(void * Controller_arg);                 // 从Controller中的pipe读取数据并发送到socket
void * controller_read_socket(void * Controller_arg);               // 从Controller中的socket读取数据并发送到所接入的Server的socket
void controller_join(baseInfo &IDInfo);                             // 控制器接入

void controller_join(baseInfo &IDInfo)
{
    Controller *ctl = new Controller(IDInfo.SerID, IDInfo.UsrID, IDInfo.socket);
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
/*
 0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|
+-+-------------+---------------------------------------------------------------+
|F|             |               |               |               |               |
|I|   opcode    |             Data length(4 Bytes)                              |
|N|(unsigned)   |               |               |               |               |
+-+-------------+---------------------------------------------------------------+
|                                                                               |
|                            other Data                                         |
|                                                                               |
+-+-------------+---------------------------------------------------------------+


*/
void * controller_read_socket(void * Controller_arg)
{
    Controller *ctl = (Controller*)Controller_arg;
    char buffer[DEFAULT_CHAR_BUFFER_SIZE];
    char sigl[5];
    unsigned char opcode;
    unsigned int len = 0, total = 0,count = 0;
    while ( recv( ctl->socket, sigl, 5 ,MSG_WAITALL) == 5 )
    {
        count = 0;
        len = 0;
        total = 0;
        opcode = sigl[0] & 0x7F;
        if ( ( sigl[0] & 0x80 ) != 0x80 )
        {
            break;
        }
        memcpy( &len, sigl + 1, 4 );
        invert( (char*)&len, 4 );
        switch (opcode)
        {
            case 0:
                while ( total < len )
                {
                    count = read( ctl->socket, buffer, len );
                    if ( count <= 0 || count > len)
                    {
                        break;
                    }
                    cout << count << endl;
                    write( ctl->ser->socket, buffer, count);
                    total += count;
                }
                cout << endl;
                break;
            default:
                cout << "unknow opcode: " << (unsigned int)opcode << endl;
                ctl->stop();
                break;
        }
    }
    if( !ctl->isClose )
    {
        ctl->stop();
    }
    
}

