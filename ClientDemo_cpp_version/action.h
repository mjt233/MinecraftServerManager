SOCKET_T getTaskAccessCmd(const char * taskID_ch);
void * acceptFile(void * arg);
void * getFileList(void * arg);

/**
 *  启动任务连接,并完成任务接入握手
 *  @param taskID_ch taskID字符串
 *  @return 成功返回任务socket,失败返回-1
*/
SOCKET_T startTask(const char * taskID_ch)
{
    SOCKET_T f_sock;            //  收发数据的socket
    char Accesscmd[64];         //  任务接入请求指令
    char buffer[1024] = {0};
    snprintf(Accesscmd, 64, "TAK%5dE%5dE%-5s", SERID, USRID, taskID_ch);
    if ( ConnectTimeOut(&f_sock, serAddr.c_str(), serPort, 10) != 1)
    {
        cout << "任务连接失败" << endl;
        close(f_sock);
        return -1;
    }
    if ( send(f_sock, Accesscmd, strlen(Accesscmd), MSG_WAITALL) != strlen(Accesscmd))
    {
        cout << "发送任务接入请求失败" << endl;
        close(f_sock);
        return -1;
    }
    if ( recv(f_sock, buffer, strnlen(taskID_ch, 20), 0) != strnlen(taskID_ch, 64) || strncmp(taskID_ch, buffer, 20))
    {
        cout << "任务接入认证失败 MSG: " << buffer << " " << strnlen(taskID_ch,20) << endl;
        close(f_sock);
        return -1;
    }
    return f_sock;
}
/** 接收服务器传来的文件
*/  
void * acceptFile(void * arg)
{
    actionAttr *at = (actionAttr*)arg;
    string str = at->data;                                  //  将char*转成string方便分析内容
    free(arg);
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
            return NULL;
        }
    }
    strncpy(name, str.substr(0,pos[0]).c_str(), 512);
    strncpy(path, str.substr(pos[0] + 1,pos[1] - pos[0] - 1).c_str(), 1024);
    strncpy(fl, str.substr(pos[1] + 1,pos[2] - pos[1] - 1).c_str(), 20);
    strncpy(id, str.substr(pos[2] + 1,str.length() - pos[2] + 1).c_str(), 20);
    if( ( f_sock = startTask(id) ) == -1 )
    {
        cout << "任务接入失败" << endl;
        return NULL;
    }
    fileLen = atoi(fl);
    char cwd[1024];
    getcwd(cwd, 1024);
    char newFilePath[2048];
    char buffer[8192] = {0};
    snprintf(newFilePath, 2048, "%s/%s/%s",cwd,path,name);
    size_t cnt,s = fileLen;
    FILE *fp = fopen(newFilePath, "wb");
    if( !fp )
    {
        cout << "文件创建失败" << endl;
        close(f_sock);
        return NULL;
    }

    // 开始接收文件
    while ( s > 0 )
    {
        cnt = recv(f_sock, buffer, s > 8192 ? 8192 : s, 0);
        if( cnt <= 0 )
        {
            close(f_sock);
            cout << "接收错误 已接收:" << fileLen - s << endl;
            fclose(fp);
            return NULL;
        }
        s -= cnt;
        fwrite(buffer, cnt, 1, fp);
    }
    send(f_sock, "OK", 2, MSG_WAITALL);
    cout << "接收完成 已接收:" << fileLen - s << " Bytes 文件位于: " << newFilePath << endl;
    fclose(fp);
    cout << endl;
    return NULL;
}



void * getFileList(void * arg)
{
    string data = ((actionAttr*)arg)->data;
    string path;
    string res = "{\"code\":200,\"data\":[";
    DIR *dp;
    frame_builder fb;
    frame_head_data fhd;
    struct dirent *dirp;
    int taskID;
    free(arg);
    int pos;
    pos = data.find('\n');
    if( pos == -1 )
    {
        return NULL;
    }

    // 解析路径,taskID
    path = data.substr(0, pos);
    taskID = atoi( data.substr(pos + 1).c_str() );

    // 准备读取
    path = s_getcwd() + "/" + path;
    cout << "读取目录: " << path << endl;

    // 任务接入
    SOCKET_T f_sock;
    f_sock = startTask(data.substr(pos + 1).c_str());

    // 打开文件夹
    dp = opendir(path.c_str());
    if( !dp )
    {
        res = "{\"code\":-2,\"msg\":\"文件夹打开失败\"}";
        fb.build(0x0, res.length());
        send(f_sock, fb.f_data, 5, MSG_WAITALL);
        send(f_sock, res.c_str(), res.length(), MSG_WAITALL);
        close(f_sock);
        return NULL;
    }
    list<string> dirList;
    list<string> fileList;
    while( dirp = readdir(dp) )
    {
        if ( !strncmp(dirp->d_name, ".", 512) || !strncmp(dirp->d_name, "..", 512))
        {
            continue;
        }
        if( dirp->d_type == 4 )
        {
            dirList.push_back(dirp->d_name);
        }else{
            fileList.push_back(dirp->d_name);
        }
    }
    list<string>::iterator i,j;
    int flag = 0;
    i = dirList.begin();
    j = fileList.begin();
    for (; i!= dirList.end(); i++ )
    {
        res += "{\"name\":\"" + *i + "\", \"type\": \"folder\"},";
        flag =1;
    }
    for (; j!= fileList.end(); j++ )
    {
        res += "{\"name\":\"" + *j + "\", \"type\": \"file\"},";
        flag = 1;
    }
    if( flag )
    {
        res[res.length() - 1] = ']';
    }
    res += "}";
    if( f_sock == -1 )
    {
        return NULL;
    }
    fb.build(0x0, res.length());
    send(f_sock, fb.f_data, sizeof(fb.f_data), MSG_WAITALL);
    send(f_sock, res.c_str(), res.length(), MSG_WAITALL);
    close(f_sock);
    return NULL;
}