/*  各类的方法定义     */
class Client;
class Controller;
class Server;


/**************************Server类**************************/

// 初始化锁和线程
Server::Server( int SerID, int UsrID, int socket ):Client::Client( SerID, UsrID, socket)
{

    // 初始化锁
    pthread_mutex_init(&sbMutex, NULL);             // 缓存字符串锁
    pthread_mutex_init(&ctlMutex, NULL);            // 控制器列表锁

    // 创建一个8K大小的缓冲字符串 16个结点 每个结点512字节的容量
    sb = buffer_create(16,512);

    // 将自己添加到服务器列表
    pthread_mutex_lock(&SerMutex);
    SerList.insert( make_pair(SerID, this ) );
    pthread_mutex_unlock(&SerMutex);

    // 接受socket数据线程
    pthread_create( &thid, NULL, server_read, this );       
}

// 将数据广播给所有接入的Controller
int Server::Broadcast(char * buf, size_t len)
{
    // 向每个Controller的管道中写入数据
    list<Controller*>::iterator itor,itorend;
    int count = 0;

    // 即将发送给控制器的信息,多线程参数结构体
    CTLMessage *CTLM;

    pthread_mutex_lock(&ctlMutex);

    itor = CTLList.begin();
    itorend = CTLList.end();
    for (;itor != itorend; itor++)
    {
        CTLM = new CTLMessage;
        memset(CTLM->buffer, 0,sizeof(CTLM->buffer));
        strncpy( CTLM->buffer, buf, len );

        CTLM->ctl = *itor;
        ++count;
        // 防止管道写满导致的阻塞
        pthread_create( &CTLM->thid, NULL, server_write_ctl_pipe, (void *)CTLM );
    }

    pthread_mutex_unlock(&ctlMutex);
    return count;
}

// 从控制器列表中移除一个控制器
void Server::removeController(Controller *ctl)
{
    pthread_mutex_lock(&ctlMutex);
    list<Controller*>::iterator i = CTLList.begin();
    cout << "Target: " << ctl->socket << endl;
    for( ; i != CTLList.end() ;)
    {
        if ( *i == ctl )
        {
            cout << "fount:" << (*i)->socket << endl;
            CTLList.erase(i++);
            break;
        }else
        {
            i++;
        }
    }
    pthread_mutex_unlock(&ctlMutex);
}


Server::~Server()
{    
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
    pthread_mutex_lock(&SerMutex);
    ser = SerList[SerID];

    // 判断服务器是否为同一用户创建
    if( ser->UsrID != UsrID )
    {
        cout << "Controller ID: "<< UsrID << " RequestID: " << ser->UsrID << endl << "$ ";
        fflush(stdout);
        pthread_mutex_unlock(&SerMutex);
        write( socket, "FORBIDDEN", 10 );
        stop();
    }else
    {
        ser->addController(this);
        write( socket, "OK", 2 );
        pthread_create( &thid, NULL, controller_read_pipe, this );
        pthread_create( &thid2, NULL, controller_read_socket, this );
        pthread_mutex_unlock(&SerMutex);
    }
}

void Controller::stop()
{
    setClose();
    close(pipe_fd[1]);
    close(pipe_fd[0]);
    shutdown( socket, SHUT_RDWR );
    SerList[SerID]->removeController(this);
}

Controller::~Controller()
{
    ser->removeController(this);
}




