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
using namespace std;
#include"../h/base/socketTool.h"
#include "socketPipe.h"
socketPipe outputPipe,inputPipe;
#include "typedef.h"
#include"../h/base/frame_builder.h"
#include "remote.h"

void ReadData();

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
    pid = fork();
    if ( pid == 0 )
    {
        // 将子进程的标准输入,标准输出和标准错误重定向到管道
        dup2(outputPipe.psocket, 1);
        dup2(outputPipe.psocket, 2);
        dup2(inputPipe.psocket, 0);
        serAttr_t *sAttr = (serAttr_t*)shmat(shm_id, 0, 0);
        L1:
        cout << "子进程: " << getpid() << endl;
        system(argv[5]);
        if( sAttr->loop == 1 )
        {
            for (size_t i = 0; i < 10; i++)
            {
                cout << "服务器已退出,将在" << 10-i <<"s后重启" << endl;
                sleep(1);
            }
            goto L1;
        }
        kill(main_pid,SIGINT);
        exit(EXIT_SUCCESS);
    }else if ( pid != 0 ){
        cout << "主进程: " << main_pid << endl;
        thread readTh(ReadData);            // 读取子进程的数据
        thread readRmTh(ReadRemoteData,argv);    // 读取服务器的数据
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
