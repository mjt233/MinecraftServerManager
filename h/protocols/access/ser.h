/********************************************************************************************************
* File name: ctl.h
* Description:  接入的服务器功能声明与定义
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: none
*********************************************************************************************************/
class WebSocket;
void * server_write_ctl_socket(void * arg);
void server_join(baseInfo &IDInfo);                                 // 服务器接入
void server_join(baseInfo &IDInfo)
{
    DEBUG_OUT("服务器接入");
    Server *ser = new Server(IDInfo.SerID, IDInfo.UsrID, IDInfo.socket);
    write( ser->socket, "OK", 2 );

    // 阻塞 等待服务器读取线程结束
    ser->th1->join();
    
    // 释放内存
    delete( ser );
    DEBUG_OUT("服务器退出");
}

void Server::server_read()
{
    char buffer[DEFAULT_CHAR_BUFFER_SIZE];
    frame_builder f_builder;
    frame_head_data head;
    int total = 0,count = 0;
    while ( recv(this->socket, head, 5, MSG_WAITALL) == 5)
    {
        total = 0;count = 0;
        f_builder.parse(head);
        if( f_builder.FIN != 1)
        {
            DEBUG_OUT("收到服务器的无效帧");
            for (size_t i = 0; i < 5; i++)
            {
                printf("%02x ",head[i]);
            }
            cout << endl;
            return ;
        }
        switch (f_builder.opcode)
        {
            case 0:
                while ( total < f_builder.length )
                {
                    count = recv(this->socket, buffer,DEFAULT_CHAR_BUFFER_SIZE - 1, 0);

                    // 读取错误时退出
                    if ( count <= 0 || count > f_builder.length ) return;

                    buffer[count] = 0;

                    if( buffer[0] == 'T' )
                    {
                        // 写入到缓冲字符串
                        this->sbMutex.lock();
                        buffer_append(&this->sb, buffer + 1);
                        this->sbMutex.unlock();
                    }

                    // 广播给所有控制器
                    Broadcast(buffer, count);
                    total += count;
                }
                break;
            case 0x1:
                status = SER_STATUS_RUNNING;
                BroadcastStatus(SER_STATUS_RUNNING);
                DEBUG_OUT("[SerID:" << SerID << "] 运行中");
                break;
            case 0x2:
                DEBUG_OUT("[SerID:" << SerID << "] 已挂起");
                status = SER_STATUS_SUSPENDED;
                BroadcastStatus(SER_STATUS_SUSPENDED);
                break;
            case 0x3:
                DEBUG_OUT("[SerID:" << SerID << "] 已停止");
                status = SER_STATUS_STOPED;
                BroadcastStatus(SER_STATUS_STOPED);
                break;
            default:
                DEBUG_OUT("服务器收到无效操作码");
                return;
                break;
        }
    }
    return;
}


void * server_write_ctl_socket(void * arg)
{
    ThParam *a = (ThParam*)arg;
    a->ctl->writeSocketData(0x0, a->msg, a->len);
    free(a->msg);
    free(arg);
}

int Server::add(Controller *ctl)
{
    cliMutex.lock();
    CTLList.push_back(ctl);
    cliMutex.unlock();
    sbMutex.lock();
    char * strbuf = buffer_get_string(&sb);
    if( !strbuf )
    {
        sbMutex.unlock();
        return 1;
    }
    sbMutex.unlock();
    char * strbuf2 = (char*)malloc(strlen(strbuf)+2);
    cout << sizeof(strbuf) << endl;
    if ( !strbuf || !strbuf2 )
    {
        cout << "malloc error" << endl;
    }
    sprintf(strbuf2,"T%s",strbuf);
    ctl->writeSocketData(0x0, strbuf2, strlen(strbuf2));
    free(strbuf);
    free(strbuf2);
    return 1;
}
