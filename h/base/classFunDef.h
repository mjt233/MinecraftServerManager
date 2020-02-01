/********************************************************************************************************
* File name: classFunDef.h
* Description:  接入服务器/控制器对象的各种方法定义
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: none
*********************************************************************************************************/

// 提前声明
void * writeWebSocket(void * arg);

/*  各类的方法定义     */

/**************************Client类**************************/
Client::Client( int SerID, int UsrID, int socket )
{
    // 初始化资源状态锁定
    this->SerID = SerID;
    this->UsrID = UsrID;
    this->socket = socket;
    pipe(pipe_fd);
}

void Client::setDisable()
{
    statMutex.lock();
    disable = true;
    statMutex.unlock();
}


void Client::setClose()
{
    statMutex.lock();
    isClose = true;
    statMutex.unlock();
}
int Client::writePipeData(frame_builder frame,const char * buf)
{
    size_t count = 0,total = 0;
    pipeIOMutex.lock();
    send(pipe_fd[1], frame.build(), 5, MSG_WAITALL);
    while ( total < frame.length )
    {
        count = write(pipe_fd[1], buf, frame.length);
        if ( count <=0 || count > frame.length )
        {
            break;
        }
        total += count;
    }
    pipeIOMutex.unlock();
    return total;
}

// 向对象的socket描述符写数据
int Client::writeSocketData(unsigned char opcode, const char * buf, unsigned int len)
{
    size_t count = 0,total = 0;
    netIOMutex.lock();
    frame_builder frame;
    frame.build(opcode, len);
    
    if ( send(this->socket, frame.f_data, 5, MSG_WAITALL) != 5)
    {
        cout << "控制发送帧时发生错误!" << endl;
        return 0;
    }
    while ( total < frame.length )
    {
        count = write(this->socket, buf + total, frame.length);
        if ( count <=0 || count > frame.length )
        {
            break;
        }
        total += count;
    }
    netIOMutex.unlock();
    return total;
}

/**************************Server类**************************/

// 初始化锁和线程
Server::Server( int SerID, int UsrID, int socket ):Client::Client( SerID, UsrID, socket)
{
    // 创建一个4k大小的缓冲字符串 8个结点 每个结点512字节的容量
    sb = buffer_create(8,512);

    // 将自己添加到服务器列表
    SerMutex.lock();
    SerList.insert( make_pair(SerID, this ) );
    SerMutex.unlock();

    // 接受socket数据线程
    th1 = new thread(&Server::server_read,this);
}

// 将数据广播给所有接入的Controller
int Server::Broadcast(char * buf, size_t len)
{
    // 向每个Controller的管道中写入数据
    ;
    int count = 0;
    cliMutex.lock();
    ThParam *msg;
    for (list<Controller*>::iterator i = CTLList.begin() ;i != CTLList.end(); i++)
    {
        ++count;
        msg = (ThParam*)malloc(sizeof(ThParam));
        msg->msg = (char*)malloc(len);
        msg->len = len;
        if ( !msg || !msg->msg)
        {
            cout << "广播失败" << endl;
            return 0;
        }
        
        msg->ctl = *i;
        strncpy(msg->msg, buf, len);
        // 防止管道写满导致的阻塞
        pthread_create(&msg->thid, NULL, server_write_ctl_socket, msg);
    }

    for (list<WebSocket*>::iterator i = WSList.begin() ;i != WSList.end(); i++)
    {
        ++count;
        msg = (ThParam*)malloc(sizeof(ThParam));
        msg->msg = (char*)malloc(len);
        msg->len = len;
        if ( !msg || !msg->msg)
        {
            cout << "广播失败" << endl;
            return 0;
        }
        
        msg->ws = *i;
        strncpy(msg->msg, buf, len);
        // 防止管道写满导致的阻塞
        pthread_create(&msg->thid, NULL, writeWebSocket, msg);
    }
    cliMutex.unlock();
    return count;
}

// 从控制器列表中移除一个控制器
int Server::remove(Controller *ctl)
{
    cliMutex.lock();
    list<Controller*>::iterator i = CTLList.begin();
    for( ; i != CTLList.end() ;)
    {
        if ( *i == ctl )
        {
            CTLList.erase(i++);
            break;
        }else
        {
            i++;
        }
    }
    cliMutex.unlock();
    return 1;
}
int closeWebSocket(WebSocket *ws);

Server::~Server()
{    

    // 将自身设置为不可用
    setDisable();

    // 断开所有的控制器socket连接,使其触发stop()
    cliMutex.lock();
    
    for ( list<Controller*>::iterator i = CTLList.begin() ; i != CTLList.end(); i++)
    {
        shutdown( (*i)->socket, SHUT_RDWR );
    }
    for ( list<WebSocket*>::iterator i = WSList.begin() ; i != WSList.end(); i++)
    {
        closeWebSocket(*i);
    }
    cliMutex.unlock();
    // 等待全部控制器断开
    while( !CTLList.empty() && !WSList.empty() )
    {
        sleep(1);
    }

    // 释放线程对象
    delete(th1);

    // 清空缓冲字符串
    buffer_reconstruct(&sb,3,3);

    // 将该Server从服务器列表中移除
    SerList.erase(SerID);
}



/**************************Controller类**************************/

Controller::Controller( int SerID, int UsrID, int socket ):Client::Client( SerID, UsrID, socket )
{
    SerMutex.lock();
    ser = SerList[SerID];

    // 判断服务器是否为同一用户创建
    if( ser->UsrID != UsrID )
    {
        fflush(stdout);
        SerMutex.unlock();
        write( socket, "FORBIDDEN", 10 );
        stop();
    }else
    {
        write( socket, "OK", 2 );
        ser->add(this);
        th2 = new thread(&Controller::controller_read_socket, this);
        SerMutex.unlock();
    }
}

void Controller::stop()
{
    setClose();
    close(pipe_fd[1]);
    close(pipe_fd[0]);
    shutdown( socket, SHUT_RDWR );
}

Controller::~Controller()
{
    ser->remove(this);
    delete(th2);
}




