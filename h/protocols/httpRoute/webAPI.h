void ls(Server * ser, HTTPRequestInfo &HQ, mutex * mtx);

void webAPI(int socket_fd, HTTPRequestInfo &HQ)
{
    string APIName = HQ.url.substr(5, HQ.url.length() - 5);
    HQ.socket_fd = socket_fd;
    HTTPResponeInfo HP;
    Server *ser;
    int SerID = atoi( HQ.GET["SerID"].c_str() );
    int UsrID = atoi( HQ.GET["UsrID"].c_str() );
    SerMutex.lock();
    if ( !checkID(SerID, UsrID) )
    {
        SerMutex.unlock();
        string msg = "{\"code\":403,\"msg\":\"验证失败\"}";
        HP.code = 200;
        HP.info = "403 Forbidden";
        HP.header["Content-Type"] = "application/json";
        HP.header["Content-Length"] = to_string(msg.length());
        HP.sendHeader(socket_fd);
        send(socket_fd, msg.c_str(), msg.length(), 0);
        return;
    }
    ser = SerList[SerID];
    SerMutex.unlock();
    mutex *mtx = new mutex;

    // Web API Route
    mtx->lock();
    if ( APIName == "ls" )
    {
        ls(ser, HQ, mtx);
    }else{
        HP.sendErrPage(socket_fd, 404, "Not Found");
    }
    mtx->unlock();
}

void ls(Server * ser, HTTPRequestInfo &HQ, mutex * mtx)
{
    HTTPResponeInfo HP;
    frame_builder fb;
    frame_head_data fhd;
    SOCKET_T sock;


    if( HQ.GET.count("path") == 0 )
    {
        HP.sendJsonMsg(HQ.socket_fd, 200, -3, "lost param", "缺少path");
        return;
    }
    sock = ser->createTask(0x2, HQ.GET["path"].c_str(), 1, mtx);
    if( sock == -1 )
    {
        HP.sendJsonMsg(HQ.socket_fd, 200, 502, "Bad Gateway", "目标服务器响应请求超时" );
        return;
    }
    char buffer[8192] = {0};
    if ( recv(sock, fhd, sizeof(frame_head_data), MSG_WAITALL) != sizeof(frame_head_data))
    {
        close(sock);
        return;
    }
    fb.analyze(fhd);
    HP.setJsonHeader();
    HP.header["Content-Length"] = to_string(fb.length);
    HP.sendHeader(HQ.socket_fd);
    size_t Rcnt = 0, Scnt = 0, cur = 0;
    while ( cur < fb.length )
    {
        if ( (Rcnt = recv(sock, buffer, 8192, 0)) <= 0 || (Scnt = send(HQ.socket_fd, buffer, Rcnt, MSG_WAITALL)) != Rcnt )
        {
            close(sock);
            return;
        }
        cur += Rcnt;
    }
    close(sock);
    return;
}