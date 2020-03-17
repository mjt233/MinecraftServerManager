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
    char input[4096];
    while ( 1 )
    {
        cin.getline(input, 4094);
	strcat(input,"\n");
        if ( send(inputPipe.psocket, input, strlen(input), MSG_WAITALL) <= 0)
        {
            cout << "写入到控制台失败！" << endl;
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
    while ( ( count = recv(outputPipe.psocket, buffer + 1, 1022, 0) ) > 0 )
    {
        SendMutex.lock();
        buffer[0] = 'T';
        buffer[count + 1] = 0;
        cout << buffer + 1;
        fb.build(0x0,count + 1);
        if ( !Connected || send(remote_socket, fb.f_data, 5, MSG_WAITALL) != 5)
        {
            cout << "远程服务器错误" << endl;
            Connected = 0;
        }

        if ( !Connected || send(remote_socket, buffer, count + 1, MSG_WAITALL) <= 0 )
        {
            cout << "远程服务器错误" << endl;
            Connected = 0;
        }
        SendMutex.unlock();
        // 不加延迟会崩溃....好神奇
        usleep(50000);
    }
    
}



void launch(const char * launch_cmd)
{
    int first = 1;
    while( first || serAttr.loop )
    {
        LAUNCH:
        setServerStatus(RUNNING);
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
            if( serAttr.suspend )
            {
                setServerStatus(SUSPENDED);
                send(outputPipe.psocket, "服务器已被挂起\n", 22, MSG_WAITALL);
                serAttr.suspend = 0;
                while ( !serAttr.launch )
                {
                    sleep(1);
                }
                serAttr.launch = 0;
                send(outputPipe.psocket, "服务器重新唤醒..启动中\n", 33, MSG_WAITALL);
                goto LAUNCH;
            }
            setServerStatus(STOPED);
            // 收到指令,立即重启
            if( serAttr.reboot )
            {
                send(outputPipe.psocket, "重启:正在启动...\n", 23, MSG_WAITALL);
                serAttr.reboot = 0;
                goto LAUNCH;
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
