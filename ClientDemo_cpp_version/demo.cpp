#include<iostream>
#include<cstdlib>
#include<string.h>
#include<string>
#include<unistd.h>
#include<thread>
#include<mutex>
#include<ctime>
#include<signal.h>
using namespace std;
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
#include"../h/base/frame_builder.h"
#include"../h/base/socketTool.h"
#include "remote.h"
#include "socketPipe.h"
void ReadData();
void ReadRemoteData();
void stop(int sign);

// 服务器socket as pipe IO
socketPipe outputPipe,inputPipe;


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
        thread readRmTh(ReadRemoteData);    // 读取服务器的数据
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
        if ( send(remote_socket, fb.f_data, 5, MSG_WAITALL) != 5)
        {
            cout << "远程服务器错误" << endl;
        }
        if (send(remote_socket, buffer, count, MSG_WAITALL) <= 0 )
        {
            cout << "远程服务器错误" << endl;
        }

        // 不加延迟会崩溃....好神奇
        usleep(100000);
    }
    
}

void ReadRemoteData()
{
    char buffer[2048];
    int count = 0,total = 0;
    frame_head_data frame;
    frame_builder fb;
    while ( recv( remote_socket, frame, 5, MSG_WAITALL ) == 5)
    {
        fb.analyze(frame);
        if ( fb.FIN != 1 )
        {
            cout << "无效数据帧信息" << endl;
            exit(EXIT_FAILURE);
        }
        count = 0;total = 0;
        switch ( fb.opcode )
        {
        case 0x0:
            while ( total < fb.length )
            {
                if ( ( count = recv(remote_socket, buffer, 2048, 0) ) <=0 )
                {
                    cout << "接收数据错误" << endl;
                }
                total += count;
                inputPipe.write(buffer, count);
                cout << "收到数据" << endl;
                for (size_t i = 0; i < count; i++)
                {
                    cout << buffer[i];
                }
                cout << endl;
                
            }
            break;
        
        default:
            cout << "无效控制码" << endl;
            break;
        }
    }
    
}