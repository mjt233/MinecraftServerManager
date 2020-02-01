/********************************************************************************************************
* File name: typedefine.h
* Description:  各种常量和对象的定义 以及函数的提前声明
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: none
*********************************************************************************************************/


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
typedef unsigned char frame_head_data[5];
class Client;
class Controller;
class Server;
class frame_builder;
class WebSocket;
/* 结构体定义  */
typedef struct baseInfo{
    int UsrID;
    int SerID;
    int socket;
}baseInfo;

typedef struct ThParam{
    pthread_t thid;
    Controller *ctl = NULL;
    WebSocket *ws = NULL;
    size_t len;
    char * msg = NULL;
}ThParam;


/* 连接路由 */
void * protocols_route(void * arg);                                 // 处理新连接
int read_request_id(int sock_fd, baseInfo &IDInfo);                 // 读取请求接入的连接基本信息
void server_join(baseInfo &IDInfo);                                 // 服务器接入
void controller_join(baseInfo &IDInfo);                             // 控制器接入


/* 业务处理 */
void * manager_server_start(void * arg);                            // 连接入口函数
void * server_read(void * arg);                                     // 从Server的socket读取数据并处理
void * server_write(void * arg);                                    // 往Server的socket写入数据
void * server_write_ctl_socket(void * arg);                         // 往Server的Controller的socket写入数据
void * controller_read_pipe(void * Controller_arg);                 // 从Controller中的pipe读取数据并发送到socket
void * controller_read_socket(void * Controller_arg);               // 从Controller中的socket读取数据并发送到所接入的Server的socket

void invert(char * buf, size_t len);

#include "frame_builder.h"



/* class定义  */
class Client{
    public:
        int SerID,              // 服务器ID
            UsrID,              // 用户ID
            socket,             // socket_fd文件描述符
            pipe_fd[2];         // 数据缓冲管道
        bool isClose = false,   // 管道与socket流是否已被关闭
             disable = false;   // 是否不可用
        mutex statMutex;        // 资源状态锁
        mutex netIOMutex;       // socket读写锁
        mutex pipeIOMutex;      // 管道读写锁
        Client( int SerID, int UsrID, int socket );
        void setDisable();
        void setClose();
        int writePipeData(frame_builder frame,const char * buf);
        int writeSocketData(unsigned char opcode, const char * buf, unsigned int len);
};

class Controller : public Client{
    public :
        thread *th1,*th2;
        Server *ser;
        mutex msgMutex;
        Controller( int SerID, int UsrID, int socket );
        ~Controller();
        void controller_read_socket();               // 从Controller中的socket读取数据并发送到所接入的Server的socket
        void stop();
};


class Server : public Client{
    public :
        thread *th1;
        mutex sbMutex,cliMutex;
        list<Controller*> CTLList;
        list<WebSocket*> WSList;
        pthread_t thid,thid2;
        stringBuffer sb;
        Server( int SerID, int UsrID, int socket );
        ~Server();
        void server_read();                                     // 从Server的socket读取数据并处理
        int add(Controller *ctl);
        int add(WebSocket *ws);
        int remove(Controller *ctl);
        int remove(WebSocket *ws);
        int Broadcast(char *buf,size_t len);
};


/* 全局变量 */
map<int,Server *> SerList;        // 服务器列表
mutex SerMutex;       // 服务器多线程列表读写锁
int SER_SOCKET;