/********************************************************************************************************
* File name: wsFileUpload.h
* Description:  通过WebSocket接受文件
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.2.6
* History: none
*********************************************************************************************************/

// 传输文件最大的buffer大小
#define MAX_FILE_TRANS_SIZE 81920
void fileUpload(HTTPFileReader &reader, int socket_fd);
void fileUpload(HTTPFileReader &reader, int socket_fd)
{
    HTTPResponeInfo HP;
    int SerID = atoi( reader.GET["SerID"].c_str() );
    int UsrID = atoi( reader.GET["UsrID"].c_str() );
    SerMutex.lock(__FILE__,__LINE__);
    if( !checkID(SerID,UsrID))
    {
        HP.sendJsonMsg(socket_fd, 200, -1, "OK", "ID验证失败");
        SerMutex.unlock();
        return;
    }
    Server *ser = SerList[SerID];
    SerMutex.unlock();
    char fileName[512] = {0},         //  文件名
         path[1024] = {0};            //  文件存放路径
    size_t len = 0;                   //  文件长度
    strncpy(fileName, reader.GET["file"].c_str(), 512);
    strncpy(path, reader.GET["path"].c_str(), 1024);
    len = atoi(reader.GET["length"].c_str());
    if( len <=0 || !strcmp(fileName,"") || !strcmp(path,"") )
    {
        HP.sendJsonMsg(socket_fd, 200, -3, "OK", "缺少参数");
        return;
    }

    if( strstr(path,"/../") || !strncmp(path + strlen(path) - 3, "..", 2) )
    {
        HP.sendJsonMsg(socket_fd, 200, -4, "OK", "路径不合法");
        return;
    }

    if( reader.fileLength == 0 )
    {
        HP.sendJsonMsg(socket_fd, 200, -3, "OK", "空文件");
        return;
    }

    mutex *mtx = new mutex;
    mtx->lock();
    SOCKET_T rec_fd ;           // 数据接收端SOCKET
    
    rec_fd = ser->startUpload(fileName, path, len, mtx);
    if ( rec_fd == -1 )
    {
        HP.sendJsonMsg(socket_fd, 200, -1, "OK", "服务器超时未响应或请求被拒绝");
        mtx->unlock();
        return ;
    }else if( rec_fd == -2 ){
        mtx->unlock();
        delete(mtx);
        HP.sendJsonMsg(socket_fd, 200, -1, "OK", "管理服务器繁忙");
        return;
    }
    char buffer[MAX_FILE_TRANS_SIZE];
    
    size_t recv_len = 0;            //  从HTTP中一次读取的字节数
    size_t total_recv = 0;          //  总共从HTTP中接收的字节数
    size_t send_len = 0;            //  一次发送给客户端的字节数
    size_t total_resend_len = 0;    //  一轮转发中总共转发给客户端的字节数
    while ( total_recv < len )
    {
        recv_len = reader.read(buffer, MAX_FILE_TRANS_SIZE);
        total_recv += recv_len;
        do
        {
            send_len = send(rec_fd, buffer + total_resend_len, recv_len, 0);
            total_resend_len += send_len;
            if( send_len <= 0 )
            {
                HP.sendJsonMsg(socket_fd, 200, -2, "OK", "中途连接断开");
                mtx->unlock();
                redTaskCount();
                return;
            }
            
        } while ( total_resend_len < recv_len );
        total_resend_len = recv_len = 0;
    }
    

    recv(rec_fd, buffer, 2, MSG_WAITALL);
    HP.sendJsonMsg(socket_fd, 200, 200, "OK", "success");
    mtx->unlock();
    redTaskCount();
}