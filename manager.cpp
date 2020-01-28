#include<iostream>
#include<cstdlib>
#include<pthread.h>
#include<string>
#include<string.h>
#include<list>
#include<map>
#include<signal.h>
using namespace std;

#include"h/base/socketTool.h"
#include"h/base/stringBuffer.h"
#include "h/base/typedefine.h"
#include "h/class/classFunDef.h"
#include "h/base/console.h"
#include "h/server.h"
int main()
{
    // 忽略处理断开的socket或管道,防止程序退出
    signal(SIGPIPE, SIG_IGN);

    // 初始化HTTP信息
    HTTP_attr_init();
    pthread_t thid;

    // 创建连接入口处理线程
    pthread_create( &thid, NULL, manager_server_start, NULL );
    
    // 控制台命令
    console_command();

    // 等待连接处理
    pthread_join(thid, NULL );
    return 0;
}