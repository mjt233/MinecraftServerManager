void ls(Server * ser, HTTPRequestInfo &HQ, mutex * mtx);
void serCtl(Server * ser, HTTPRequestInfo &HQ, mutex * mtx);
void getfile(Server * ser, HTTPRequestInfo &HQ, mutex * mtx);

void webAPI(int socket_fd, HTTPFileReader &HQ)
{
    #ifdef linux
    struct timeval tm = {5,0};
    #endif // linux
    #ifdef WIN32
    int tm = 5000;
    #endif // WIN32
    // setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(tm));
    // setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &tm, sizeof(tm));
    string APIName = HQ.url.substr(5, HQ.url.length() - 5);
    HQ.socket_fd = socket_fd;
    HTTPResponeInfo HP;
    Server *ser;
    int SerID = atoi( HQ.GET["SerID"].c_str() );
    int UsrID = atoi( HQ.GET["UsrID"].c_str() );
    SerMutex.lock(__FILE__,__LINE__);
    if ( !checkID(SerID, UsrID) )
    {
        SerMutex.unlock(__FILE__,__LINE__);
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
    SerMutex.unlock(__FILE__,__LINE__);
    mutex *mtx = new mutex;

    // Web API Route
    mtx->lock();
    if (        APIName == "ls" )
    {
        ls(ser, HQ, mtx);
    }else if(   APIName == "ctl" ){
        serCtl(ser, HQ, mtx);
        mtx->unlock();
        return;
    }else if(   APIName == "fileupload" ){
        fileUpload(HQ,HQ.socket_fd);
    }else if(   APIName == "getfile"){
        getfile(ser, HQ, mtx);
    }else{
        HP.sendErrPage(socket_fd, 404, "Not Found");
    }
    mtx->unlock();
}

void getfile(Server * ser, HTTPRequestInfo &HQ, mutex * mtx)
{
    HTTPResponeInfo HP;
    if(HQ.GET.count("file") == 0)
    {
        HP.sendJsonMsg(HQ.socket_fd, 200, -3, "OK", "缺少参数:file");
        return;
    }
    SOCKET_T recv_fd;
    recv_fd = ser->createTask(OPCODE_VIEWFILE, HQ.GET["file"].c_str(), HQ.GET["file"].length(), mtx);
    if( recv_fd == -1 )
    {
        HP.sendJsonMsg(HQ.socket_fd, 200, -2, "OK", "服务器响应超时");
        redTaskCount();
        return;
    }else if(recv_fd == -2){
        HP.sendJsonMsg(HQ.socket_fd, 200, -1, "OK", "管理服务器繁忙");
        redTaskCount();
        return;
    }
    frame_builder fb;
    frame_head_data fd;
    size_t cnt,             //  单次读取的字节数
           total = 0;       //  总共读取的字节数
    char buffer[BIG_CHAR_BUFFER_SIZE] = {0};
    cnt = recv(recv_fd, fd, FRAME_HEAD_SIZE, MSG_WAITALL);
    if( cnt != FRAME_HEAD_SIZE )
    {
        HP.sendJsonMsg(HQ.socket_fd, 200, -2, "OK", "服务器通信错误");
        redTaskCount();
        return;
    }
    fb.parse(fd);
    if( fb.opcode == 0x1 )
    {
        HP.sendJsonMsg(HQ.socket_fd, 200, -2, "OK", "服务器打开文件失败");
        redTaskCount();
        return;
    }
    string filename = HQ.GET["file"];
    if( HQ.GET.count("text") )
    {
        HP.header["Content-Type"] = getFileExtension("1.txt");
    }else
    {
        HP.header["Content-Type"] = getFileExtension(filename);
    }
    HP.header["Content-Length"] = to_string(fb.length);
    if( HP.sendHeader(HQ.socket_fd) < 0)
    {
        close(HQ.socket_fd);
        close(recv_fd);
        redTaskCount();
        return;
    }
    while ( total < fb.length )
    {
        cnt = recv(recv_fd, buffer, BIG_CHAR_BUFFER_SIZE, 0);
        if( cnt > BIG_CHAR_BUFFER_SIZE || cnt <= 0 || send(HQ.socket_fd, buffer, cnt, MSG_WAITALL) != cnt )
        {
            close(recv_fd);
            close(HQ.socket_fd);
            redTaskCount();
            return;
        }
        total += cnt;
    }
    redTaskCount();
    return;
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
    fb.parse(fhd);
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