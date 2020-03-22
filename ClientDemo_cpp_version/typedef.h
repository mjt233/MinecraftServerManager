/********************************   常量声明    *************************/

#define RUNNING 0x1
#define SUSPENDED 0x2
#define STOPED  0x3

/******************************** 数据类型声定义 *************************/
    // 多线程参数结构体
    // thread对象还得管生命周期,直接用pthread反而方便一点
typedef struct {
    char data[2048];
    size_t len;
}actionAttr;

typedef struct {
    int loop = 1;       // 是否崩溃自动重启标识
    int reboot = 0;     // 重启标识
    int suspend = 0;       // 挂起标识
    int launch = 0;     // 指令启动标识
    int status = 0;     // 服务器状态
    int backup = 0;     // 备份中标识
}serAttr_t;      // 服务器属性结构体

/******************************** 全局变量声明 **********************/

pid_t   pid,                // 子进程ID
        main_pid;           // 主进程ID
string serAddr = "";        // 管理服务器地址
unsigned short serPort = 0; // 管理服务器端口
serAttr_t serAttr;          // 服务器属性结构体
unsigned char SERSTATUS;    // 服务器当前状态
int SERID,USRID;            // SerID and UsrID
string launch_cmd = "";
string BAK_RES = "";
string BAK_DEST = "";

/************************** 函数声明 ****************************/

void invert(char * buf, size_t len);    // 字符串倒置
string s_getcwd();                      // 取运行路径
void closeServer(int sign);
void Exit(int sign);
void setStatus(int statusCode);         // 设置并更新服务器状态

/************************** 函数定义 **************************/


string s_getcwd()
{
    string res;
    char buffer[2048];
    getcwd(buffer, 2048);
    res = buffer;
    return res;
}


void invert(char * buf, size_t len)
{
    char t;
    for (size_t i = 0; i < len/2; i++)
    {
        t = buf[i];
        buf[i] = buf[ len-i-1 ];
        buf[len-i-1] = t;
    }
}


void Exit(int sign)
{
    serAttr.loop = 0;
    inputPipe.write("stop\n",5);
    cout << "Interrupt" << endl;
    sleep(1);
    if(sign)
    {
        kill(pid, SIGABRT);
    }
    exit(EXIT_FAILURE);
}
