/********************************************************************************************************
* File name: classFunDef.h
* Description:  接入服务器/控制器对象的各种方法定义
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: 
*   2019.2.6    移除管道相关函数 增加服务器接收文件函数
*   2019.1.28   创建
*********************************************************************************************************/

/**
 *  验证用户ID和服务器实例ID是否相匹配
 *  执行该函数前注意加锁(SerMutex)
 *  @return 匹配返回1 否则0
*/
int checkID(int SerID, int UsrID)
{
    if( SerList.count(SerID) == 0 ){
        return 0;
    }

    Server *ser = SerList[SerID];
    if( ser->UsrID == UsrID ){
        return 1;
    }else{
        return 0;
    }
}

// 提前声明 用于向WebSocket虚拟终端转发服务器的控制台IO
void * writeWebSocket(void * arg);

/*  各类的方法定义     */

/**************************Client类**************************/
Client::Client( int SerID, int UsrID, int socket )
{
    // 初始化资源状态锁定
    this->SerID = SerID;
    this->UsrID = UsrID;
    this->socket = socket;
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


// 向对象的socket描述符写数据
int Client::writeSocketData(unsigned char opcode, const char * buf, unsigned int len)
{
    size_t count = 0,total = 0;
    netIOMutex.lock();
    frame_builder frame;
    frame.build(opcode, len);
    
    if ( send(this->socket, frame.f_data, 5, MSG_WAITALL) != 5)
    {
        netIOMutex.unlock();
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
    // 创建一个16k大小的缓冲字符串 8个结点 每个结点512字节的容量
    sb = buffer_create(32,512);

    // 将自己添加到服务器列表
    SerMutex.lock(__FILE__,__LINE__);
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
            cliMutex.unlock();
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
            cliMutex.unlock();
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

/** 用于向服务器发出"创建任务"请求,
 *  @param otherInfo 附加任务信息
 *  @param len 附加任务信息字符长度
 *  @param mtx 任务锁
 *  @return 成功返回任务socket通道,失败返回-1,任务拥堵-2
 */ 
SOCKET_T Server::createTask(unsigned char opcode, const char * otherInfo, size_t len, mutex * mtx)
{
    if( !incTaskCount() )
    {
        return -2;
    }
    if( len > 1000 )
    {
        redTaskCount();
        return -1;
    }
    int taskID;
    char buffer[1024] = {0};
    SOCKET_T rec_fd;
    srand(time(NULL));
    taskID = rand() % 99999 + 1;

    // 随机生成任务ID
    while ( taskList.count(taskID) )
    {
        taskID = (rand() % 99999) + 1;
    }
    statMutex.lock();
    taskList[taskID] = -1;
    taskMutex[taskID] = mtx;
    statMutex.unlock();
    snprintf(buffer, 1024, "%s\n%d", otherInfo, taskID);
    writeSocketData(opcode, buffer, strnlen(buffer, 1024));
    for (size_t i = 0; i < 10; i++)
    {
        for (size_t j = 0; j < 50; j++)
        {
            usleep(10000);
            statMutex.lock();
            if( taskList[taskID] != -1 )
            {
                rec_fd = taskList[taskID];
                statMutex.unlock();
                return rec_fd;
            }
            statMutex.unlock();
        }
    }
    statMutex.lock();
    taskList.erase(taskID);
    statMutex.unlock();
    redTaskCount();
    return -1;
}


/** 用于向服务器发出"发送文件"请求,
 *  @param name 文件名
 *  @param path 服务器接收文件的路径
 *  @param length 文件大小(Byte)
 *  @param mtx 任务锁
 *  @return 成功返回接收文件socket,失败返回-1
 */ 
SOCKET_T Server::startUpload(const char * name, const char * path, size_t length, mutex * mtx)
{
    char info[1024];
    snprintf(info, 1024, "%s\n%s\n%ld", name, path, length);
    return createTask(0x1, info, strnlen(info, 1024), mtx);

}

int Server::BroadcastStatus(unsigned char statusCode)
{
    char msg[128] = {0};
    snprintf(msg, 128, "S{\"status\":%d}", statusCode);
    return Broadcast(msg, strnlen(msg, 128));
}

Server::~Server()
{   

    SerMutex.lock(__FILE__,__LINE__);

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
    SerMutex.unlock();
}



/**************************Controller类**************************/

Controller::Controller( int SerID, int UsrID, int socket ):Client::Client( SerID, UsrID, socket )
{
    SerMutex.lock(__FILE__,__LINE__);
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
    shutdown( socket, SHUT_RDWR );
}

Controller::~Controller()
{
    ser->remove(this);
    delete(th2);
}
