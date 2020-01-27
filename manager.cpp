#include<iostream>
#include<cstdlib>
#include<pthread.h>
#include<string>
#include<string.h>
#include<list>
#include<map>
#include<signal.h>
#include"h/socketTool.h"
#include"h/stringBuffer.h"

using namespace std;
/* 全局变量定义 */
#include "typedefine.h"
#include "http.h"
#include "classFunDef.h"
#include "console.h"
#include "server.h"

int main()
{
    // 忽略处理断开的socket或管道,防止程序退出
    signal(SIGPIPE, SIG_IGN);

    // 初始化HTTP信息
    HTTP_attr_init();
    pthread_t thid;

    // 创建连接入口处理线程
    pthread_create( &thid, NULL, entrance, NULL );
    
    // 控制台命令
    console_command();

    // 等待连接处理
    pthread_join(thid, NULL );
    return 0;
}