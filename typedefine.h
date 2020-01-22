#define DEFAULT_CHAR_BUFFER_SIZE 1024               // 默认的buffer大小
#define SERVER 0                                    // 服务器TAG
#define CONTROLLER 1                                // 控制器TAG
#define COMMAND 2                                   // 快捷命令TAG
#define HTTP_GET 3                                  // HTTP请求
#define UNKNOW_TYPE -1                              // 未知TAG
#define SERID_FORMAT_INCOREET -2                    // 服务器ID格式错误
#define USRID_FORMAT_INCOREET -3                    // 用户ID格式错误
#define SERID_UNEXIST -4                            // 服务器ID不存在
#define SERID_EXIST -5                              // 服务器ID已存在
#define MEMORY_OVERFLOW -10                         // 内存不足 无法申请内存


class Client;
class Controller;
class Server;


/* 结构体定义  */
typedef struct baseInfo{
    int UsrID;
    int SerID;
    int socket;
}baseInfo;

typedef struct CTLMessage{
    pthread_t thid;
    Controller *ctl;
    char buffer[DEFAULT_CHAR_BUFFER_SIZE];
}CTLMessage;


/* 连接路由 */
void * process_connect(void * arg);                                 // 处理新连接
int read_request_id(int sock_fd, baseInfo &IDInfo);                 // 读取请求接入的连接基本信息
void server_join(baseInfo &IDInfo);                                 // 服务器接入
void controller_join(baseInfo &IDInfo);                             // 控制器接入


/* 业务处理 */
void * entrance(void * arg);                                        // 连接入口函数
void * server_read(void * arg);                                     // 从Server的socket读取数据并处理
void * server_write(void * arg);                                    // 往Server的socket写入数据
void * server_write_ctl_pipe(void * arg);                           // 往Server的Controller的pipe_fd[1]写入数据
void * controller_read_pipe(void * Controller_arg);                 // 从Controller中的pipe读取数据并发送到socket
void * controller_read_socket(void * Controller_arg);               // 从Controller中的socket读取数据并发送到所接入的Server的socket







/* class定义  */
class Client{
    public:
        int SerID,              // 服务器ID
            UsrID,              // 用户ID
            socket,             // socket_fd文件描述符
            pipe_fd[2];         // 数据缓冲管道
        bool isClose = false,   // 管道与socket流是否已被关闭
             disable = false;   // 是否不可用
        pthread_mutex_t statMutex;  // 资源状态锁
        Client( int SerID, int UsrID, int socket )
        {
            // 初始化资源状态锁定
            pthread_mutex_init(&statMutex,NULL);
            this->SerID = SerID;
            this->UsrID = UsrID;
            this->socket = socket;
            pipe(pipe_fd);
        }
        void setDisable()
        {
            pthread_mutex_lock(&statMutex);
            disable = true;
            pthread_mutex_unlock(&statMutex);
        }
        void setClose()
        {
            pthread_mutex_lock(&statMutex);
            isClose = true;
            pthread_mutex_unlock(&statMutex);
        }
};

class Controller : public Client{
    public :
        pthread_t thid,thid2;
        Server *ser;
        Controller( int SerID, int UsrID, int socket );
        ~Controller();
        void stop();
};


class Server : public Client{
    public :
        list<Controller*> CTLList;
        pthread_mutex_t sbMutex;
        pthread_mutex_t ctlMutex;
        pthread_t thid,thid2;
        stringBuffer sb;
        Server( int SerID, int UsrID, int socket );
        ~Server();
        void addController(Controller *ctl)
        {
            pthread_mutex_lock(&ctlMutex);
            CTLList.push_back(ctl);
            pthread_mutex_unlock(&ctlMutex);
        }
        void removeController(Controller *ctl);
        int Broadcast(char *buf,size_t len);
};


/* 全局变量 */
map<int,Server *> SerList;        // 服务器列表
pthread_mutex_t SerMutex;       // 服务器多线程列表读写锁
int SER_SOCKET;