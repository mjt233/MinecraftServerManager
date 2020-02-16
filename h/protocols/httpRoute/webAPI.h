void ls(Server * ser, HTTPRequestInfo &HQ, mutex * mtx);
void serCtl(Server * ser, HTTPRequestInfo &HQ, mutex * mtx);

void webAPI(int socket_fd, HTTPFileReader &HQ)
{
    string APIName = HQ.url.substr(5, HQ.url.length() - 5);
    HQ.socket_fd = socket_fd;
    HTTPResponeInfo HP;
    Server *ser;
    int SerID = atoi( HQ.GET["SerID"].c_str() );
    int UsrID = atoi( HQ.GET["UsrID"].c_str() );
    SerMutex.lock(__FILE__,__LINE__);
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
    }else if( APIName == "ctl" ){
        serCtl(ser, HQ, mtx);
        mtx->unlock();
        return;
    }else if( APIName == "fileupload" ){
        fileUpload(HQ,HQ.socket_fd);
    }else{
        HP.sendErrPage(socket_fd, 404, "Not Found");
    }
    mtx->unlock();
}

void serCtl(Server * ser, HTTPRequestInfo &HQ, mutex * mtx)
{
    HTTPResponeInfo HP;
    frame_builder fb;
    frame_head_data fhd;
    string type;
    if( HQ.POST.count("type") == 0 )
    {
        HP.sendJsonMsg(HQ.socket_fd, 200, -3, "lost param", "缺少type");
        return;
    }
    type = HQ.POST["type"];
    int isClose = ( type == "stop" || type == "suspend" || type == "reboot" || type == "force-shutdown");
    if( type == "launch" && ser->status == SER_STATUS_RUNNING )
    {
        HP.sendJsonMsg(HQ.socket_fd, 200, -4, "error", "已经在运行啦!");
        return;
    }
    if( type == "launch" && ser->status == SER_STATUS_STOPED )
    {
        HP.sendJsonMsg(HQ.socket_fd, 200, -4, "error", "只能启动被挂起的服务器");
        return;
    }
    if( isClose && ser->status != SER_STATUS_RUNNING)
    {
        HP.sendJsonMsg(HQ.socket_fd, 200, -4, "error", "服务器已经关掉啦~");
        return;
    }
    if( isClose )
    {
        DEBUG_OUT("[SerID:" << ser->SerID << "] 关闭中");
        ser->BroadcastStatus(SER_STATUS_STOPPING);
        ser->status = SER_STATUS_STOPPING;
    }
    ser->writeSocketData(0x3, type.c_str(), type.length());
    HP.sendJsonMsg(HQ.socket_fd, 200, 200, "OK", "OK");
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
    }else if( sock == -2 ){
        HP.sendJsonMsg(HQ.socket_fd, 503, 503, "Service Unavailable" ,"服务器已达到最大任务负载数");
        return;
    }
    char buffer[8192] = {0};
    if ( recv(sock, fhd, sizeof(frame_head_data), MSG_WAITALL) != sizeof(frame_head_data))
    {
        close(sock);
        redTaskCount();
        return;
    }
    fb.analyze(fhd);
    HP.setJsonHeader();
    HP.header["Content-Length"] = to_string(fb.length);
    if ( HP.sendHeader(HQ.socket_fd) <= 0 )
    {
        close(sock);
        redTaskCount();
        return;
    }
    size_t Rcnt = 0, Scnt = 0, cur = 0;
    while ( cur < fb.length )
    {
        if ( (Rcnt = recv(sock, buffer, 8192, 0)) <= 0 || (Scnt = send(HQ.socket_fd, buffer, Rcnt, MSG_WAITALL)) != Rcnt )
        {
            close(sock);
            redTaskCount();
            return;
        }
        cur += Rcnt;
    }
    close(sock);
    redTaskCount();
    return;
}