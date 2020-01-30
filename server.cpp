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
    if ( InitConnect(&sock,(char*)"127.0.0.1",6636) !=0 )
    {
        cout << "connect error" << endl;
    }
    write(sock,(char*)"SER00001E12345E",15);
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
        
        send(sock, fb.build(0,input.length()), 5, MSG_WAITALL);
        cout << send(sock, input.c_str(), input.length(), MSG_WAITALL) << endl;
    }
    
    return 0;
}



void recvinfo(int sock)
{
    char buffer[1024] ={0};
    frame_builder fb;
    frame_head_data frame;
    int count,total,need_read;
    while ( ( count = read(sock, buffer, 1024) ) > 0 )
    {
        for (size_t i = 0; i < count; i++)
        {
            printf("%02x ",(unsigned char)buffer[i]);
        }
        cout << endl;
    }
    
    // while ( recv(sock, frame, 5, MSG_WAITALL) == 5 )
    // {
    //     count = 0;total = 0;
    //     fb.analyze(frame);
    //     if ( fb.FIN != 1 )
    //     {
    //         cout << "帧错误" << endl;
    //         for (size_t i = 0; i < 5; i++)
    //         {
    //             printf("%02x ", fb.f_data[i]);
    //         }
    //         cout << endl;
    //         exit;
    //     }
    //     printf("opcode:%d length:%d\n",fb.opcode,fb.length);
    //     while ( total < fb.length )
    //     {
    //         need_read = fb.length - total;
    //         cout << "nead_read :" << need_read << endl;
    //         count = read(sock, buffer, need_read > 1024 ? 1024: need_read );
    //         if(count <=0 || count > need_read)
    //         {
    //             cout << "read错误" << endl;
    //             exit(1);
    //         }
    //         buffer[count] = 0;
    //         total += count;
    //         cout << buffer << endl;
    //         memset(buffer,0,1024);
    //         printf("read count:%d\n",count);
    //     }
        
    // }
}
