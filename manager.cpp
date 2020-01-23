#include<iostream>
#include<cstdlib>
#include<pthread.h>
#include<string>
#include<string.h>
#include<list>
#include<map>
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
    mime_type_init();
    pthread_t thid;
    pthread_create( &thid, NULL, entrance, NULL );
    
    console_command();
    // 等待连接处理
    pthread_join(thid,NULL );
    return 0;
}