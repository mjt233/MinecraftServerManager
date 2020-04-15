
void * serverControl(void * arg);
void setServerStatus(unsigned char status);



void * serverControl(void * arg)
{
    string data = ((actionAttr*)arg)->data;
    cout << "收到服务器控制指令: " << data << endl;
    if( data == "force-shutdown" )
    {
        kill(pid, SIGABRT);
    }else if( data == "stop" ){
        send(inputPipe.psocket, "stop\n", 5, MSG_WAITALL);
    }else if( data == "reboot" ){
        serAttr.reboot = 1;
        send(inputPipe.psocket, "stop\n", 5, MSG_WAITALL);
    }else if( data == "suspend" ){
        serAttr.suspend = 1;
        send(inputPipe.psocket, "stop\n", 5, MSG_WAITALL);
    }else if( data == "launch" ){
        serAttr.launch = 1;
    }
    return NULL;
}

void setServerStatus(unsigned char status)
{
    frame_builder fb;
    fb.build(status,0);
    send(remote_socket, fb.f_data, sizeof(fb.f_data), MSG_WAITALL);
    SERSTATUS = status;
}
