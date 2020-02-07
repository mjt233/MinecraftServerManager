
mutex ConnectedMutex;
int Connected = 0;

int remote_socket;
int AccessServer(const char * addr,unsigned short port,int SerID,int UsrID);

// 从远程管理器读取数据
void ReadRemoteData(char const *argv[]);

void acceptFile(const char * data, size_t len);

// 成功1 失败0
int AccessServer(const char * addr,unsigned short port,int SerID,int UsrID)
{
    // if( InitConnect(&remote_socket, addr, port) != 0 )
    // {
    //     cout << "connect error" << endl;
    //     return 0;
    // }
    if( ConnectTimeOut(&remote_socket, addr, port, 5) != 1 )
    {
        cout << "connect error" << endl;
        return 0;
    }
    char buf[30];
    snprintf(buf,16,"SER%5dE%5dE",SerID,UsrID);
    send(remote_socket, buf, 15, MSG_WAITALL);
    recv(remote_socket, buf, 2, MSG_WAITALL);
    buf[2] = 0;
    cout << buf[0] << buf[1] << endl;
    if( strcmp(buf,"OK") ){
        int count = 0;
        cout << "接入失败,原因:";
        count = recv(remote_socket, buf + 2, 27, 0);
        if( count <= 0 )
        {
            cout << "原因获取失败QAQ" << endl;
            return 0;
        }else{
            buf[count+2] = 0;
            cout << buf << endl;
            return 0;
        }
    }
    Connected = 1;
    return 1;
}


void ReadRemoteData(char const *argv[])
{
    char buffer[2048];
    int count = 0,total = 0;
    frame_head_data frame;
    frame_builder fb;
    START :
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
            case 0x1:
                if ( ( count = recv( remote_socket, buffer, fb.length, MSG_WAITALL ) )== fb.length )
                {
                    buffer[count] = 0;
                    acceptFile(buffer, count);
                }
                break;
            default:
                cout << "无效控制码" << endl;
                shutdown(remote_socket, SHUT_RDWR);
                goto END;
                break;
            }
    }
    END:
    Connected = 0;
    close(remote_socket);
    int i = 0;
    do
    {
        cout << "连接已断开..准备重连(第" << ++i << "次)" << endl;
        sleep(5);
    } while ( !AccessServer(argv[1],atoi(argv[2]),atoi(argv[3]),atoi(argv[4]))  );
    Connected = 1;
    std::cout << "重连成功" << std::endl;
    
    goto START;
}

void acceptFile(const char * data, size_t len)
{
    string str = data;                                      //  将char*转成string方便分析内容
    char name[512],path[1024],id[20] = {0},fl[20] = {0};    //  分割后的内容
    size_t fileLen;                                         //  文件长度
    int taskID;                                             //  任务ID
    int pos[3] = {0,0,0};                                   //  3个换行符所在的位置
    int f_sock;                                             //  收发数据的socket
    char Accesscmd[64];                                     //  任务接入请求指令
    for (size_t i = 0; i < 3; i++)
    {
        if( ( pos[i] = str.find('\n', i ? pos[i-1] + 1: 0) ) == -1 )
        {
            return;
        }
    }
    strncpy(name, str.substr(0,pos[0]).c_str(), 512);
    strncpy(path, str.substr(pos[0] + 1,pos[1] - pos[0] - 1).c_str(), 1024);
    strncpy(fl, str.substr(pos[1] + 1,pos[2] - pos[1] - 1).c_str(), 20);
    strncpy(id, str.substr(pos[2] + 1,str.length() - pos[2] + 1).c_str(), 20);
    fileLen = atoi(fl);
    snprintf(Accesscmd, 64, "TSF%5dE%5dE%-5s", SERID, USRID, id);
    cout << "Accesscmd : " << Accesscmd << endl;
    if ( ConnectTimeOut(&f_sock, serAddr.c_str(), serPort, 10) != 1)
    {
        cout << "接收文件连接失败" << endl;
        close(f_sock);
        return;
    }
    if ( send(f_sock, Accesscmd, strlen(Accesscmd), MSG_WAITALL) != strlen(Accesscmd))
    {
        cout << "接收文件请求失败" << endl;
        close(f_sock);
        return;
    }

    char buffer[8192] = {0};
    size_t cnt,s = fileLen;
    if ( recv(f_sock, buffer, strnlen(id, 20), 0) != strnlen(id, 64) || strncmp(id, buffer, 20))
    {
        cout << "接收文件连接认证失败 MSG: " << buffer << " " << strnlen(id,20) << endl;
        close(f_sock);
        return;
    }
    while ( s > 0 )
    {
        cnt = recv(f_sock, buffer, s > 8192 ? 8192 : s, 0);
        if( cnt <= 0 )
        {
            close(f_sock);
            cout << "接收错误" << endl;
            return;
        }
        for (size_t i = 0; i < cnt; i++)
        {
            cout << buffer[i];
        }
        s -= cnt;
    }
    cout << endl;
    
}