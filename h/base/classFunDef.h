/********************************************************************************************************
* File name: classFunDef.h
* Description:  接入服务器/控制器对象的各种方法定义
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: none
*********************************************************************************************************/


/*  各类的方法定义     */


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
    list<Controller*>::iterator itor,itorend;
    int count = 0;
    ctlMutex.lock();
    CTLMessage *msg;
    itor = CTLList.begin();
    itorend = CTLList.end();
    for (;itor != itorend; itor++)
    {
        ++count;
        msg = (CTLMessage*)malloc(sizeof(CTLMessage));
        msg->ctl = *itor;
        msg->msg = buf;

        // 防止管道写满导致的阻塞
        pthread_create(&msg->thid, NULL, server_write_ctl_pipe, msg);
        
    }
    ctlMutex.unlock();
    return count;
}

// 从控制器列表中移除一个控制器
void Server::removeController(Controller *ctl)
{
    ctlMutex.lock();
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
    ctlMutex.unlock();
}


Server::~Server()
{    
    // 释放线程对象
    delete(th1);

    // 将自身设置为不可用
    disable = true;

    // 断开所有的控制器socket连接,使其触发stop()
    list<Controller*>::iterator i = CTLList.begin();
    for (  ; i != CTLList.end(); i++)
    {
        shutdown( (*i)->socket, SHUT_RDWR );
    }

    // 等待全部控制器断开
    while( !CTLList.empty() )
    {
        sleep(1);
    }

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
        th1 = new thread(&Controller::controller_read_pipe, this);
        ser->addController(this);
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
    ser->removeController(this);
    delete(th1);
    delete(th2);
}




