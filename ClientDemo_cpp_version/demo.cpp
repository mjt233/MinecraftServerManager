#include<iostream>
#include<cstdlib>
#include<string.h>
#include<string>
#include<unistd.h>
#include<thread>
#include<mutex>
#include<ctime>
#include<signal.h>
#include<dirent.h>
#include<list>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/wait.h>
using namespace std;
#include"../h/base/socketTool.h"
#include "socketPipe.h"
socketPipe outputPipe,inputPipe;
#include "typedef.h"
#include"../h/base/frame_builder.h"
#include "remote.h"

void ReadData();
void launch(const char * launch_cmd);
// 主函数
int main(int argc, char const *argv[])
{
    signal(SIGINT, Exit);
    main_pid = getpid();
    if ( argc != 6 )
    {
        cout << "[usage] ./demo [addr] [port] [SerID] [UsrID] \"[Server Start Command]\"" << endl;
        return 1;
    }
    InitData(argv);
    // 接入服务器
    if ( !AccessServer(argv[1],atoi(argv[2]),atoi(argv[3]),atoi(argv[4])))
    {
        Exit(0);
        return 1;
    }
    thread launchTh(launch, argv[5]);                    // 服务器线程
    thread readTh(ReadData);                    // 读取子进程的数据
    thread readRmTh(ReadRemoteData,argv);       // 读取服务器的数据
    string input;
    while ( 1 )
    {
        cin >> input;
        input+="\n";
        if ( send(inputPipe.psocket, input.c_str(), input.length(), MSG_WAITALL) <= 0)
        {
            exit(EXIT_SUCCESS);
        }
    }
    Exit(0);
    return 0;
}



void ReadData()
{
    char buffer[1024] = {0};
    int count;
    frame_builder fb;
    while ( ( count = recv(outputPipe.psocket, buffer, 1023, 0) ) > 0 )
    {
        buffer[count] = 0;
        cout << buffer;
        fb.build(0x0,count);
        if ( !Connected || send(remote_socket, fb.f_data, 5, MSG_WAITALL) != 5)
        {
            cout << "远程服务器错误" << endl;
            Connected = 0;
        }

        if ( !Connected || send(remote_socket, buffer, count, MSG_WAITALL) <= 0 )
        {
            cout << "远程服务器错误" << endl;
            Connected = 0;
        }

        // 不加延迟会崩溃....好神奇
        usleep(50000);
    }
    
}



void launch(const char * launch_cmd)
{
    int first = 1;
    while( first || serAttr.loop )
    {
        pid = fork();
        if(pid == 0){
            dup2(outputPipe.psocket, 1);
            dup2(outputPipe.psocket, 2);
            dup2(inputPipe.psocket, 0);
            execl("/bin/bash","bash","-c",launch_cmd,nullptr);
            goto FAIL;
        }else if(pid != -1){
            waitpid(pid, NULL, 0);

            // 服务器暂时挂起
            if( serAttr.hang )
            {
                serAttr.hang = 0;
                while ( !serAttr.launch )
                {
                    sleep(1);
                }
                serAttr.launch = 0;
            }

            // 收到指令,立即重启
            if( serAttr.reboot )
            {
                serAttr.reboot = 0;
                continue;
            }

            // 崩溃自重启
            char str[512];
            if ( serAttr.loop )
            {
                for (size_t i = 0; i < 10; i++)
                {
                    snprintf(str, 512, "服务器已停止,将在%lds后重启\n",10-i);
                    send(outputPipe.psocket, str, strnlen(str, 512), MSG_WAITALL);
                    sleep(1);
                }
            }
        }else{
            FAIL:
            cout << "创建子进程失败" << endl;
            exit(EXIT_FAILURE);
        }
    }
}