/*  各类的方法定义     */
class Client;
class Controller;
class Server;
// Server类的构造函数
Server::Server( int SerID, int UsrID, int socket ):Client::Client( SerID, UsrID, socket)
{

    // 初始化锁
    pthread_mutex_init(&sbMutex, NULL);             // 缓存字符串锁
    pthread_mutex_init(&ctlMutex, NULL);            // 控制器列表锁

    // 创建一个8K大小的缓冲字符串 16个结点 每个结点512字节的容量
    sb = buffer_create(16,512);

    pthread_create( &thid, NULL, server_read, this );       // 数据接受线程
}

// Server类的析构函数
Server::~Server()
{    
    // 将该Server从服务器列表中移除
    pthread_mutex_lock(&SerMutex);
    SerList.erase(SerID);
    pthread_mutex_unlock(&SerMutex);
}
// Controller类的构造函数
Controller::Controller( int SerID, int UsrID, int socket ):Client::Client( SerID, UsrID, socket )
{
    pthread_mutex_lock(&SerMutex);
    Server *ser = SerList[SerID];
    pthread_mutex_unlock(&SerMutex);

    // 判断服务器是否为同一用户创建
    if( ser->UsrID != UsrID )
    {
        pthread_mutex_unlock(&SerMutex);
        write( socket, "FORBIDDEN", 10 );
        close( socket);
    }
    pthread_create( &thid, NULL, controller_read_pipe, this );

}


// 将数据广播给所有接入的Controller
int Server::Broadcast()
{
    // 向每个Controller的管道中写入数据
    list<Controller>::iterator itor,itorend;
    int count = 0;

    // 即将发送给控制器的信息,多线程参数结构体
    CTLMessage *CTLM;

    pthread_mutex_lock(&ctlMutex);

    itor = CTLList.begin();
    itorend = CTLList.end();
    for (;itor != itorend; itor++)
    {
        CTLM = new CTLMessage;
        strncpy( CTLM->buffer, buffer, DEFAULT_CHAR_BUFFER_SIZE );

        // itor不是Controller*型, &*itor才是Controller*型 好骚
        CTLM->ctl = &*itor;
        ++count;
        // 防止管道写满导致的阻塞
        pthread_create( &CTLM->thid, NULL, server_write_ctl_pipe, (void *)CTLM );
    }

    pthread_mutex_unlock(&ctlMutex);
    return count;
}