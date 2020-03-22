
int remote_socket;
#include "action.h"
#define CREATE_TASK_THREAD(func) \
    if ( ( count = recv( remote_socket, buffer, fb.length, MSG_WAITALL ) )== fb.length )    \
    {                                                                                       \
        buffer[count] = 0;                                                                  \
        at = (actionAttr*)malloc(sizeof(actionAttr));                                       \
        strncpy(at->data, buffer, 2048);                                                    \
        pthread_create(&thid, NULL, func, (void*)at);                                       \
    }else{ goto END; }                                                                      \

mutex SendMutex;
int Connected = 0;

int AccessServer(const char * addr,unsigned short port,int SerID,int UsrID);

// 从远程管理器读取数据
void ReadRemoteData();



// 成功1 失败0
int AccessServer(const char * addr,unsigned short port,int SerID,int UsrID)
{
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


void ReadRemoteData()
{
    char buffer[2048];
    int count = 0,total = 0;
    frame_head_data frame;
    frame_builder fb;
    pthread_t thid;
    actionAttr *at;
    START :
    while ( recv( remote_socket, frame, 5, MSG_WAITALL ) == 5)
    {
        fb.parse(frame);
        if ( fb.FIN != 1 )
        {
            cout << "无效数据帧信息" << endl;
            break;
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
                }
                break;
            case 0x1:
                CREATE_TASK_THREAD(acceptFile)
                break;
            case 0x2:
                CREATE_TASK_THREAD(getFileList)
                break;
            case 0x3:
                CREATE_TASK_THREAD(serverControl)
                break;
            case 0x4:
                CREATE_TASK_THREAD(viewfile)
                break;
            case 0x5:
                CREATE_TASK_THREAD(fileRename);
                break;
            case 0x6:
                cout << "bak" << endl;
                CREATE_TASK_THREAD(backup);
                break;
            case 0x7:
                CREATE_TASK_THREAD(getBackups);
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
    } while ( !AccessServer(serAddr.c_str(),serPort, SERID, USRID)  );
    Connected = 1;
    std::cout << "重连成功" << std::endl;
    cout << (int)SERSTATUS << endl;
    // 跟进当前状态
    setServerStatus(SERSTATUS);
    goto START;
}

