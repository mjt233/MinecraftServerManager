/********************************************************************************************************
* File name: typedefine.h
* Description:  各种常量和对象的定义 以及函数的提前声明
* Author: mjt233@qq.com
* Version: 1.1
* Date: 2019.1.28
* History: 
*   2019.2.6    增加HTTP_POST 常量标识
*   2019.1.28   创建
*********************************************************************************************************/
/*
服务器/控制器数据帧定义
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
0x1: 控制端请求发送文件
*/
#define DEFAULT_CHAR_BUFFER_SIZE 2048               // 默认的buffer大小
#define BIG_CHAR_BUFFER_SIZE 8192                   // 默认的buffer大小
#define SERVER 0                                    // 服务器TAG
#define CONTROLLER 1                                // 控制器TAG
#define TASK 2                                      // 异步任务请求
#define HTTP_GET 3                                  // HTTP请求
#define HTTP_POST 4                                 // HTTP POST请求
#define UNKNOW_TYPE -1                              // 未知TAG
#define SERID_FORMAT_INCOREET -2                    // 服务器ID格式错误
#define USRID_FORMAT_INCOREET -3                    // 用户ID格式错误
#define SERID_UNEXIST -4                            // 服务器ID不存在
#define SERID_EXIST -5                              // 服务器ID已存在
#define SER_STATUS_RUNNING  1                       // 服务器运行中
#define SER_STATUS_STOPPING 2                       // 服务器停止中
#define SER_STATUS_SUSPENDED    3                   // 服务器已挂起
#define SER_STATUS_LAUNCHING    4                   // 服务器启动中
#define SER_STATUS_STOPED    5                      // 服务器启动中

#define OPCODE_CONSOLE 0X0
#define OPCODE_FILETRANS 0x1
#define OPCODE_LS 0X2
#define OPCODE_CTL 0x3
#define OPCODE_VIEWFILE 0x4
#define OPCODE_RENAME 0x5
#define OPCODE_BACKUP 0x6
#define OPCODE_GETBACKUPS 0x7

#define FRAME_HEAD_SIZE 5
// #define DEBUG_OUT_N("LockSuccess");DEBUG_OUT_N(__FILE__);DEBUG_OUT(__LINE__);
typedef unsigned char frame_head_data[FRAME_HEAD_SIZE];
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
// void * controller_read_pipe(void * Controller_arg);                 // 从Controller中的pipe读取数据并发送到socket
void * controller_read_socket(void * Controller_arg);               // 从Controller中的socket读取数据并发送到所接入的Server的socket

void invert(char * buf, size_t len);

#include "frame_builder.h"



/* class定义  */

class myMutex{
    public:
        mutex *mtx;
        char file[2048],ul_f[2048], tl_f[2048];
        int line,ul_l, tl_l;
        myMutex(mutex *mtx){
            this->mtx = mtx;
        }
        void lock(const char *fileName,int Line){
            mtx->lock();
            snprintf(file, 2048, "%s",fileName);
            line = Line;
        }
        void unlock(const char *fileName,int Line){
            mtx->unlock();
            snprintf(ul_f, 2048, "%s",fileName);
            ul_l = Line;
        }
        bool try_lock(const char *fileName,int Line){
            bool res = mtx->try_lock();
            if( res )
            {
                snprintf(tl_f, 2048, "success %s",fileName);
            }else
            {
                snprintf(tl_f, 2048, "fail %s",fileName);
            }
            tl_l = Line;
            return res;
        }
        string toString(){
            string res;
            res = "last lock:";
            res += file;
            res += " at line " + to_string(line) + "\nlast unlock:";
            res += ul_f;
            res += " at line " + to_string(ul_l) + "\nlast trylock:" + tl_f + " at line " + to_string(tl_l);
            return res;
        }
};

class Client{
    public:
        int SerID,              // 服务器ID
            UsrID,              // 用户ID
            socket;             // socket_fd文件描述符
        bool isClose = false,   // 管道与socket流是否已被关闭
             disable = false;   // 是否不可用
        mutex statMutex;        // 资源状态锁
        mutex netIOMutex;       // socket读写锁
        Client( int SerID, int UsrID, int socket );
        void setDisable();
        void setClose();
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
        list<Controller*> CTLList;      // 控制器列表
        list<WebSocket*> WSList;        // WebSocket会话列表
        map<int,SOCKET_T> taskList;     // 任务会话列表
        map<int,mutex*> taskMutex;      // 任务会话同步锁
        int status;                     // 服务器状态
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
        int BroadcastStatus(unsigned char statusCode);
        SOCKET_T startUpload(const char * name, const char * path, size_t length, mutex * mtx);
        SOCKET_T createTask(unsigned char opcode, const char * otherInfo, size_t len, mutex * mtx);
};


/* 全局变量 */
map<int,Server *> SerList;        // 服务器列表
mutex SerMutexx;       // 服务器多线程列表读写锁
mutex SrcMutex;         // 资源锁
myMutex SerMutex(&SerMutexx);
int SER_SOCKET;
unsigned int cur_task_count = 0;    // 当前已创建的任务会话数
unsigned int max_task_count = 100;  // 最大的任务会话数

/**
 * 任务数增加1
 * @return 成功返回1,达到上限增加失败返回0
*/
int incTaskCount()
{
    SrcMutex.lock();
    if( cur_task_count >= max_task_count )
    {
        SrcMutex.unlock();
        return 0;
    }else{
        cur_task_count++;
        SrcMutex.unlock();
        return 1;
    }
}

int redTaskCount()
{
    SrcMutex.lock();
    if( cur_task_count <= 0 )
    {
        SrcMutex.unlock();
        return 0;
    }else{
        cur_task_count--;
        SrcMutex.unlock();
        return 1;
    }
}