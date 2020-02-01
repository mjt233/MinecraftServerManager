/********************************************************************************************************
* File name: localSocket.h
* Description:  为方便移植,利用socket代替管道
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.31
* History: none
*********************************************************************************************************/



#define LOCALHOST ( (char*)"127.0.0.1" )

int local_socket_ser,cli_socket_output,cli_socket_input,ser_socket_output,ser_socket_input;
unsigned short local_port;
void readPipe();
void InitLocalSocket();
void redirectLocalSocket();
void dataRedTh1();
void dataRedTh2();

thread *th0,*th1,*th2;
void InitLocalSocket()
{
    int success = 0;
    int t;
    time_t timer;
    srand(time(&timer));
    while ( success!=1 )
    {
        local_port = rand()%64535+1000;
        if( InitConnect(&t,LOCALHOST,local_port) == 0 )
        {
            close(local_port);
            continue;
        }
        success++;
    }
    InitServer(&local_socket_ser,LOCALHOST,local_port);
    th0 = new thread(redirectLocalSocket);
    cout << "local port: " << local_port << endl;
    if ( InitConnect(&cli_socket_output,LOCALHOST,local_port) )
    {
        cout << "Local Socket Error" << endl;
        exit(EXIT_FAILURE);
    }
    if ( InitConnect(&cli_socket_input,LOCALHOST,local_port) )
    {
        cout << "Local Socket Error" << endl;
        exit(EXIT_FAILURE);
    }
    th1 = new thread(dataRedTh1);
    th2 = new thread(dataRedTh2);
}

void redirectLocalSocket()
{
    struct sockaddr_in sockaddr;
    socklen_t len = sizeof(sockaddr);
    if ( ( ser_socket_input = accept(local_socket_ser, (struct sockaddr *)&sockaddr, &len) ) == -1 ){
        cout << "local socket accept error" << endl;
        exit(EXIT_FAILURE);
    }
    if ( ( ser_socket_output = accept(local_socket_ser, (struct sockaddr *)&sockaddr, &len) ) == -1 ){
        cout << "local socket accept error" << endl;
        exit(EXIT_FAILURE);
    }
}


void dataRedTh1()
{
    char buffer[2048];
    int count;
    while ( ( count = recv(ser_socket_input, buffer, 2048, 0) ) > 0 )
    {
        cout << "count : " << count << endl;
        send(ser_socket_input, buffer, count, MSG_WAITALL);
    }
    cout << "socket_input退出!!!" << endl;
}

void dataRedTh2()
{
    char buffer[2048];
    int count;
    while ( ( count = recv(ser_socket_output, buffer, 2048, 0) ) > 0 )
    {
        cout << "count : " << count << endl;
        send(ser_socket_output, buffer, count, MSG_WAITALL);
    }
    cout << "socket_output退出!!!" << endl;
}