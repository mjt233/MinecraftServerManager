void ls(Server * ser, HTTPRequestInfo &HQ, mutex * mtx);

void webAPI(int socket_fd, HTTPRequestInfo &HQ)
{
    string APIName = HQ.url.substr(5, HQ.url.length() - 5);
    HTTPResponeInfo HP;
    Server *ser;
    int SerID = atoi( HQ.POST["SerID"].c_str() );
    int UsrID = atoi( HQ.POST["UsrID"].c_str() );
    SerMutex.lock();
    if ( !checkID(SerID, UsrID) )
    {
        string msg = "{\"code\":403,\"msg\":\"验证失败\"}";
        HP.code = 403;
        HP.info = "403 Forbidden";
        HP.header["Content-Type"] = "application/json";
        HP.header["Content-Length"] = to_string(msg.length());
        HP.sendHeader(socket_fd);
        send(socket_fd, msg.c_str(), msg.length(), 0);
        return;
    }
    ser = SerList[SerID];
    SerMutex.unlock();
    mutex mtx;
    mtx.lock();
    if ( APIName == "ls" )
    {
        ls(ser, HQ, &mtx);
    }
}

void ls(Server * ser, HTTPRequestInfo &HQ, mutex * mtx)
{
    ser->createTask(0x2, "/", 1, mtx);
}