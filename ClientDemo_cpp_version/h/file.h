
mutex lsMutex;
void * acceptFile(void * arg);
void * viewfile(void * arg);
void * getFileList(void * arg);
void * fileRename(void * arg);
void * backup(void * arg);
void * getBackups(void * arg);

void * getBackups(void * arg)
{
    actionAttr *at = (actionAttr*)arg;
    string data = at->data;
    free(arg);

    SOCKET_T send_fd;
    send_fd = startTask(data.c_str());
    if( send_fd == -1 )
    {
        cout << "创建task失败" << endl;
        return NULL;
    }
    frame_builder fb;
    DIR *dp = opendir(BAK_DEST.c_str());
    dirent *de;
    int tag = 0;
    string t;
    string res;
    string list = "[";
    while ( (de = readdir(dp)) )
    {
        if ( !strncmp(de->d_name, ".", 512) || !strncmp(de->d_name, "..", 512))
        {
            continue;
        }
        t = de->d_name;
        list += t + ",";
        tag = 1;
    }
    if( tag )
    {
        list[list.length()-1] = ']';
    }else{
        list = "[]";
    }

    res = "{\"code\":200,\"list\":" + list + "}";
    fb.build(0, res.length());
    send(send_fd, fb.f_data, FRAME_HEAD_SIZE, MSG_WAITALL);
    send(send_fd, res.c_str(), res.length(), MSG_WAITALL);
    return NULL;
}

void * backup(void * arg)
{
    if( serAttr.backup == 1 ){
        outputPipe.write("备份任务未完成",21);
        return NULL;
    }
    int sto = dup(STDOUT_FILENO);
    serAttr.backup = 1;
    string destination =  BAK_DEST + "/" +to_string( time(NULL) );
    outputPipe.write("==============开始执行备份===============\n",48);
    copy_dir(BAK_RES.c_str(),destination.c_str());
    outputPipe.write("==============备份执行完成===============\n",48);
    serAttr.backup = 0;
    return NULL;
}

void * fileRename(void * arg)
{
    frame_builder fb;
    string oldName,newName,path;
    vector<string> info;
    char taskID[10] = {0};

    // 读取参数
    actionAttr *at = (actionAttr*)arg;
    string data(at->data);
    cout << data << endl;
    free(arg);

    // 参数解析
    path = s_getcwd();
    info = str_split(data,"\n");
    oldName = path + "/" + info[0];
    newName = path + "/" + info[1];
    strcpy(taskID, info[2].c_str());

    // task接入
    SOCKET_T send_fd;
    send_fd = startTask(taskID);

    if( send_fd == -1 )
    {
        cout << "创建task失败" << endl;
        return NULL;
    }
    printf("oldname:%s newname:%s \n",oldName.c_str(),newName.c_str());
    if( rename(oldName.c_str(),newName.c_str() ) < 0)
    {
        fb.build(1,0);
    }else{
        fb.build(0,0);
    }
    send(send_fd, fb.f_data, FRAME_HEAD_SIZE, MSG_WAITALL);
    close(send_fd);
    return NULL;
}

void * viewfile(void * arg )
{
    actionAttr *at = (actionAttr*)arg;
    string str(at->data);
    free(arg);
    char fileName[1024] = {0};
    char taskID[10] = {0};
    int pos;
    pos = str.find("\n");
    if( pos == -1 )
        return NULL;
    strncpy(fileName, str.c_str(), pos);
    strncpy(taskID, str.c_str() + pos + 1, str.length() - pos - 1);
    SOCKET_T send_fd;
    send_fd = startTask(taskID);
    if( send_fd == -1 )
    {
        cout << "task接入失败" << endl;
        return NULL;
    }
    frame_builder fb;
    FILE *fp = fopen(fileName, "rb");
    if( fp == NULL )
    {
        fb.opcode = 0x1;
        fb.length = 0;
        fb.build();
        send(send_fd, fb.f_data, FRAME_HEAD_SIZE, MSG_WAITALL);
        close(send_fd);
        return NULL;
    }
    // 取文件大小
    fseek(fp, 0, SEEK_END);
    fb.length = ftell(fp);
    fb.opcode = 0x0;
    fseek(fp, 0, SEEK_SET);
    if ( send(send_fd, fb.build(), FRAME_HEAD_SIZE, MSG_WAITALL) != FRAME_HEAD_SIZE)
    {
        cout << "发送文件信息失败" << endl;
        close(send_fd);
        fclose(fp);
        return NULL;
    }
    char buffer[8192];
    size_t cnt;                 //  记录一次读取的大小
    size_t a = fb.length;       //  剩余需要读取的大小
    while ( !feof(fp) && a)
    {
        cnt = fread(buffer, 1, a > 8192 ? 8192 : a, fp);
        a -= cnt;
        if ( send(send_fd, buffer, cnt, MSG_WAITALL) != cnt)
        {
            cout << "发送文件中断" << endl;
            close(send_fd);
            fclose(fp);
            return NULL;
        }
    }
    close(send_fd);
    fclose(fp);
    return NULL;
}


/** 接收服务器传来的文件
*/  
void * acceptFile(void * arg)
{
    cout << "开始传输文件" << endl;
    actionAttr *at = (actionAttr*)arg;
    string str = at->data;                                  //  将char*转成string方便分析内容
    free(arg);
    char name[512],path[1024],id[20] = {0},fl[20] = {0};    //  分割后的内容
    size_t fileLen;                                         //  文件长度
    int taskID;                                             //  任务ID
    int pos[3] = {0,0,0};                                   //  3个换行符所在的位置
    int f_sock;                                             //  收发数据的socket
    char Accesscmd[64];                                     //  任务接入请求指令
    vector<string> info;
    info = str_split(str,"\n");
    strncpy(name, info[0].c_str(), 512);
    strncpy(path, info[1].c_str(), 1024);
    strncpy(fl, info[2].c_str(), 20);
    strncpy(id, info[3].c_str(), 20);
    if( ( f_sock = startTask(id) ) == -1 )
    {
        cout << "任务接入失败" << endl;
        return NULL;
    }
    fileLen = atoi(fl);
    char cwd[512];
    getcwd(cwd, 1024);
    char newFilePath[2048];
    char buffer[MAX_FILE_TRANS_SIZE] = {0};
    snprintf(newFilePath, sizeof(newFilePath), "%s/%s/%s",cwd,path,name);
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
        cnt = recv(f_sock, buffer, s > MAX_FILE_TRANS_SIZE ? MAX_FILE_TRANS_SIZE : s, 0);
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
    lsMutex.lock();
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
        lsMutex.unlock();
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

    if( f_sock == -1 )
    {
        lsMutex.unlock();
        cout << "ls接入失败" << endl;
        return nullptr;
    }
    // 打开文件夹
    dp = opendir(path.c_str());
    if( !dp )
    {
        lsMutex.unlock();
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
    struct stat statbuf;
    for (; i!= dirList.end(); i++ )
    {
        res += "{\"name\":\"" + *i + "\", \"type\": \"folder\",\"size\":\"-\"},";
        flag =1;
    }
    for (; j!= fileList.end(); j++ )
    {
        stat((path + *j).c_str(), &statbuf);
        res += "{\"name\":\"" + *j + "\", \"type\": \"file\",\"size\":" + to_string(statbuf.st_size) + "},";
        flag = 1;
    }
    if( flag )
    {
        res[res.length() - 1] = ']';
    }else{
        res +="]";
    }
    res += "}";
    if( f_sock == -1 )
    {
        lsMutex.unlock();
        closedir(dp);
        return NULL;
    }
    lsMutex.unlock();
    fb.build(0x0, res.length());
    send(f_sock, fb.f_data, sizeof(fb.f_data), MSG_WAITALL);
    send(f_sock, res.c_str(), res.length(), MSG_WAITALL);
    close(f_sock);
    closedir(dp);
    return NULL;
}

