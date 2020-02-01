#include<iostream>
#include<cstdlib>
#include<string>
#include<string.h>
#include<thread>
using namespace std;
typedef unsigned char frame_head_data[5];
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
#include "h/base/socketTool.h"
#include "h/base/frame_builder.h"
void recvinfo(int sock);
int main(int argc, char const *argv[])
{
    frame_builder fb;
    frame_head_data frame;
    char buffer[1024] ={0};
    fb.build(0,5);
    int sock;

    if( argc != 3 )
    {
        cout << "usage:./client [addr] [access command]"<<endl;
        return 1;
    }

    if ( InitConnect(&sock,argv[1],6636) !=0 )
    {
        cout << "connect error" << endl;
    }
    write(sock,argv[2],15);
    recv(sock, buffer, 2, MSG_WAITALL);
    if(strcmp(buffer,"OK"))
    {
        cout << "接入失败" << endl;
        return 0;
    }else{
        cout << "接入成功" << endl;
    }
    thread th(recvinfo,sock);
    string input;
    while ( 1 )
    {
        cin >> input;
        input += "\n";
        send(sock, fb.build(0,input.length()), 5, MSG_WAITALL);
        send(sock, input.c_str(), input.length(), MSG_WAITALL);
    }
    
    return 0;
}



void recvinfo(int sock)
{
    char buffer[1024] ={0};
    frame_builder fb;
    frame_head_data frame;
    int count,total,need_read;
    while ( recv(sock, frame, 5, MSG_WAITALL) == 5 )
    {
        total = 0;
        fb.analyze(frame);
        switch (fb.opcode)
        {
        case 0x0:
                while ( total < fb.length )
                {
                    count = recv(sock, buffer, 1023, 0);
                    buffer[count] = 0;
                    total += count;
                    cout << buffer;
                }
            break;
        
        default:
            cout << "无效帧" << endl;
            break;
        }
        
        cout << endl;
    }
    exit(EXIT_SUCCESS);
}
