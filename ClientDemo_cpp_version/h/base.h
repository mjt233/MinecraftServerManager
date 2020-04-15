void readConfig();
vector<string> str_split(string str,string pattern);
SOCKET_T startTask(const char * taskID_ch);

char * toLow(char * str,size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if( str[i] >='A' && str[i] <='Z' )
        {
            str[i] +='a' - 'A';
        }
    }
    return str;
}

void readConfig()
{
    FILE *fp = fopen("mcsm.conf","r");
    if( fp == NULL )
    {
        cout << "丢失配置文件" << endl;
        cout << "文件名:mcsm.conf\n文件内容示例:\n";
        string content = "# MCSM接入服务器地址\n"
        "server=127.0.0.1\n"
        "# MCSM接入服务器端口\n"
        "port=6636\n"
        "# MC服务器标识\n"
        "SerID=1\n"
        "# MC服务器管理员标识\n"
        "UsrID=12345\n"
        "# MC服务器启动命令\n"
        "launch=cd server && java -jar minecraft_server.1.12.2.jar --nogui\n"
        "# 备份源\n"
        "backupSource=server/world\n"
        "# 备份到\n"
        "backupDestination=server/backups\n"
        "# 崩溃自动重启\n"
        "autoReboot=1\n";
    }
    char buffer[1024];
    string str,key,value;
    int pos,len;
    while ( !feof(fp) )
    {
        fgets(buffer,1024,fp);
        len = strlen(buffer);
        if(buffer[0] == '#' || buffer[0] == '\n')
        {
            continue;
        }
        if( buffer[len - 1] == '\n' )
        {
            buffer[len - 1] = 0;
        }
        str = buffer;
        pos = str.find('=');
        if( pos == -1 )
        {
            cout << "错误配置:" << str << endl;
            continue;
        }
        key = str.substr(0,pos);
        value = str.substr(pos+1);
        if( key == "server" ){
            serAddr = value;
        }else if( key == "port" ){
            serPort = atoi(value.c_str());
        }else if( key == "SerID" ){
            SERID = atoi(value.c_str());
        }else if( key == "UsrID" ){
            USRID = atoi(value.c_str());
        }else if( key == "launch"){
            launch_cmd = value;
        }else if( key == "backupSource"){
            BAK_RES = value;
        }else if( key == "backupDestination"){
            BAK_DEST = value;
        }else{
            cout << "无效配置:" << str << endl;
        }
    }
    
}

vector<string> str_split(string str,string pattern)
{
    vector<string> res;
    string t;
    int p1,p2;
    p1 = p2 = 0;
    while ( (p2 = str.find(pattern, p1)) != -1 )
    {
        t = str.substr(p1, p2 - p1);
        res.push_back(t);
        p1 = p2 = p2 + pattern.length();
    }
    res.push_back(str.substr(p1));
    return res;
}

SOCKET_T getTaskAccessCmd(const char * taskID_ch);

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

void copy_dir(string source, string target)
{
    DIR *dp= opendir(source.c_str());
    dirent *de;
    string src,dest;
    FILE *out,*in;
    char buffer[8192];
    char str[4096];
    size_t len;
    if( access(target.c_str(),0) < 0 )
    {
        if ( mkdir(target.c_str(), 0777) < 0)
        {
            sprintf(str,"创建%s失败\n",target.c_str());
            cout << str;
            outputPipe.write(str,strlen(str));
            return;
        }
    }
    if( dp == NULL )
    {
        cout << "打开文件夹" << source << "失败" << endl;
        return;
    }
    while ( (de = readdir(dp)) )
    {
        if( !strcmp(de->d_name,".") || !strcmp(de->d_name,"..") )
        {
            continue;
        }
        if( de->d_type == 4 )
        {
            string t(de->d_name);
            copy_dir(source + "/" + t, target + "/" + t);
            continue;
        }
        src = source + "/" + de->d_name;
        dest = target + "/" + de->d_name;
        sprintf(str,"%-60s -> %s\n",src.c_str(), dest.c_str());
        in = fopen(src.c_str(),"rb");
        out = fopen(dest.c_str(),"wb");
        outputPipe.write(str,strlen(str));
        if(in == NULL)
        {
            sprintf(str,"打开文件%s失败\n",src.c_str());
            cout << "打开文件" << src <<"失败" << endl;
            outputPipe.write(str,strlen(str));
            continue;
        }
        if(out == NULL)
        {
            sprintf(str,"打开文件%s失败\n",dest.c_str());
            cout << "打开文件" << dest <<"失败" << endl;
            outputPipe.write(str,strlen(str));
            continue;
        }
        while ( (len = fread(buffer, 1, 8192, in)) )
        {
            fwrite(buffer, 1, len, out);
        }
        fclose(in);
        fclose(out);
	closedir(dp);
    }
}
