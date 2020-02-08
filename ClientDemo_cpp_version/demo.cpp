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
using namespace std;

string serAddr;
unsigned short serPort;
int SERID,USRID;
// 解决大小端序问题
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
#include "typedef.h"
#include"../h/base/frame_builder.h"
#include"../h/base/socketTool.h"
#include "socketPipe.h"
socketPipe outputPipe,inputPipe;
#include "remote.h"

void ReadData();
void stop(int sign);


pid_t pid,      // 子进程ID
    main_pid;   // 主进程ID

// 主函数
int main(int argc, char const *argv[])
{
    signal(SIGINT, stop);
    main_pid = getpid();
    if ( argc != 6 )
    {
        cout << "[usage] ./demo [addr] [port] [SerID] [UsrID] \"[Server Start Command]\"" << endl;
        return 1;
    }
    serAddr = argv[1];
    serPort = atoi( argv[2] );
    SERID = atoi( argv[3] );
    USRID = atoi( argv[4] );
    if ( !AccessServer(argv[1],atoi(argv[2]),atoi(argv[3]),atoi(argv[4])))
    {
        return 1;
    }
    
    if ( (pid = fork()) == 0 )
    {
        dup2(outputPipe.psocket, 1);
        dup2(outputPipe.psocket, 2);
        dup2(inputPipe.psocket, 0);
        // execlp("java","java","-jar","minecraft_server.1.12.2.jar",NULL);
        // system("cd server && java -jar minecraft_server.1.12.2.jar");
        system(argv[5]);
        kill(main_pid,SIGINT);
        exit(EXIT_SUCCESS);
    }else{
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
    return 0;
}

void stop(int sign)
{
    cout << "Interrupt" << endl;
    inputPipe.write("stop\n",5);

    kill(pid,SIGINT);
    exit(EXIT_SUCCESS);
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
