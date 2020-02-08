/********************************************************************************************************
* File name: wsFileUpload.h
* Description:  通过WebSocket接受文件
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.2.6
* History: none
*********************************************************************************************************/
void wsFileUpload(HTTPRequestInfo &HQ, int socket_fd);
void wsFileUpload(HTTPRequestInfo &HQ, int socket_fd)
{
    WebSocket ws(HQ, socket_fd);
    if ( !ws.wsHandShake() )
    {
        ws.close();
        return;
    }
    int SerID = atoi( ws.HTTPRequest.GET["SerID"].c_str() );
    int UsrID = atoi( ws.HTTPRequest.GET["UsrID"].c_str() );
    if( SerID == 0 || UsrID == 0 )
    {
        ws.die( (char *)"无效wsID",10);
        return;
    }
    SerMutex.lock();

    // 判断服务器目标是否存在
    if( !SerList.count(SerID) )
    {
        ws.die((char *)"Unexist Server",14);
        SerMutex.unlock();
        return;
    }
    Server *ser = SerList[SerID];

    // 需要请求者ID与服务器创建者ID需一致
    if ( ser->UsrID != UsrID )
    {
        SerMutex.unlock();
        ws.die((char *)"Authentication failed", 21);
        return;
    }
    SerMutex.unlock();

    char data[1024] = {0};
    string dataStr;
    int cnt, pos[3] = {0,0,0};
    char *p;
    char name[512],path[1024];
    size_t len;
    wsHeadFrame wsf;
    ws.readHeadFrame(wsf);
    cout << "msg操作码:" << (unsigned int)wsf.opcode << endl;
    if( wsf.payload_length >= 1023 ){
        ws.die((char*)"Data Too Long", 13);
    }
    if ( (cnt = ws.readData(wsf, data, 1024)) <= 0 )
    {
        cout << "握手失败" << endl;
        return;
    }
    data[cnt] = 0;
    dataStr = data;
    int i = 0;
    if( ( pos[0] = dataStr.find('\n') )!= -1 ){
        strncpy(name, dataStr.substr(0, pos[0]).c_str(), 512);
    }

    if( pos[0] < dataStr.length() - 1 ){
        pos[1] = dataStr.find('\n', pos[0] + 1);
        strncpy(path, dataStr.substr(pos[0] + 1, pos[1] - pos[0] -1 ).c_str() , 1024);
    }else{
        ws.writeData(0x2, "format error", 12);
        return;
    }

    len = atoi( dataStr.substr(pos[1] + 1, dataStr.length() - 1).c_str() );
    if( len <= 0 ){
        ws.writeData(0x2, "format error", 12);
        return;
    }

    // 获取接收端SOCKET
    mutex mtx;
    mtx.lock();
    SOCKET_T rec_fd = ser->startUpload(name, path, len, &mtx);
    if ( rec_fd == -1 )
    {
        ws.die((char*)"Target server unable to accept file", 35);
        mtx.unlock();
        return ;
    }
    char buffer[204800];
    
    size_t recv_len = 0;
    size_t ws_recv = 0;
    size_t a;
    string proc;
    while ( ws_recv < len )
    {
        if ( !ws.readHeadFrame(wsf) )
        {
            ws.die("帧读取失败",15);
            mtx.unlock();
            return;
        }
        while ( recv_len < wsf.payload_length )
        {
            cnt = ws.readData(wsf, buffer, 204800);
            proc = to_string(cnt);
            a = proc.length();
            ws.writeData(0x1, proc.c_str(), a);
            if( cnt <= 0 || send(rec_fd, buffer, cnt, MSG_WAITALL) != cnt)
            {
                ws.close();
                shutdown(rec_fd,SHUT_RDWR);
                mtx.unlock();
                return;
            }
            recv_len += cnt;
        }
        recv_len = 0;
        ws_recv += wsf.payload_length;
    }
    

    recv(rec_fd, buffer, 2, MSG_WAITALL);
    ws.writeData(0x1, "OK", 2);
    ws.close();
    mtx.unlock();
}