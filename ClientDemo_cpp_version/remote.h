
mutex ConnectedMutex;
int Connected = 0;

int remote_socket;
int AccessServer(const char * addr,unsigned short port,int SerID,int UsrID);

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