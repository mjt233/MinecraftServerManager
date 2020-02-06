/********************************************************************************************************
* File name: ctl.h
* Description:  接入的控制器功能声明与定义
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: none
*********************************************************************************************************/
void invert(char * buf, size_t len);
void controller_join(baseInfo &IDInfo);

void controller_join(baseInfo &IDInfo)
{
    DEBUG_OUT("控制器接入");
    Controller *ctl = new Controller(IDInfo.SerID, IDInfo.UsrID, IDInfo.socket);
    ctl->th2->join();
    delete( ctl );
    DEBUG_OUT("控制器退出");
}


// void Controller::controller_read_pipe()
// {
//     char buffer[DEFAULT_CHAR_BUFFER_SIZE];
//     frame_builder f_builder;
//     frame_head_data head;
//     while ( 1 )
//     {
//         recv(pipe_fd[0], head, 5, MSG_WAITALL);
//         f_builder.analyze(head);
//         if( this->isClose )
//         {
//             break;
//         }
//         switch ( f_builder )
//         {
//         case /* constant-expression */:
//             /* code */
//             break;
        
//         default:
//             break;
//         }
        
//     }
    
//     while ( read( this->pipe_fd[0], buffer, 1024 ) > 0)
//     {
//         if( this->isClose )
//         {
//             break;
//         }
//         write( this->socket, buffer, strlen(buffer) );
//         memset(buffer, 0, sizeof(buffer));
//     }
// }
/*
|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|
+-+-------------+---------------------------------------------------------------+
|F|             |                                                               |
|I|   opcode    |             Data length(4 Bytes)                              |
|N|(unsigned)   |                                                               |
+-+-------------+---------------------------------------------------------------+
|                                                                               |
|                            other Data                                         |
|                                                                               |
+-+-------------+---------------------------------------------------------------+
opcode操作码定义
0x0: 服务器控制台纯文本传输
0x1: 

*/
void Controller::controller_read_socket()
{
    Controller *ctl = this;
    char buffer[DEFAULT_CHAR_BUFFER_SIZE];
    frame_builder f_builder;
    frame_head_data f_data;
    unsigned int total = 0,count = 0;
    while ( recv( ctl->socket, f_data, 5 ,MSG_WAITALL) == 5 )
    {
        count = 0;
        total = 0;
        f_builder.analyze(f_data);
        if ( f_builder.FIN != 1 )
        {
            if (DEBUG_MODE)
            {
                printf("无效帧数据\nFIN: 0x%02x First Byte:0x%02x\n帧信息:",f_builder.FIN, f_builder.f_data[0] );
                for (size_t i = 0; i < 5; i++)
                {
                    printf("%02x ",f_data[i]);
                }
                cout << endl;
            }
            
            break;
        }
        if (DEBUG_MODE)
        {
            printf("FIN:0x%d opcode:0x%d len:%d\n",f_builder.FIN, f_builder.opcode, f_builder.length );
        }
        switch (f_builder.opcode)
        {
            case 0:
                while ( total < f_builder.length )
                {
                    count = recv( ctl->socket, buffer, f_builder.length , 0);
                    if ( count <= 0 || count > f_builder.length)
                    {
                        break;
                    }
                    
                    ctl->ser->writeSocketData(0x0, buffer, count);
                    total += count;
                }
                cout << endl;
                break;
            default:
                DEBUG_OUT("控制器:无效操作码");
                ctl->stop();
                break;
        }
    }
    if( !ctl->isClose )
    {
        ctl->stop();
    }

}

