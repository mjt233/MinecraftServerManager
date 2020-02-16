/********************************************************************************************************
* File name: respone.h
* Description:  HTTP模块的请求体解析
* Author: mjt233@qq.com
* Version: 1.5
* Date: 2019.2.15
* History: 
*   2019.2.15   增加通过POST进行单个文件传输功能
*   2019.2.6    增加POST简单解析
*   2019.1.28   创建
*********************************************************************************************************/



#ifndef HTTP_REQUEST

#endif // !HTTP_REQUEST

#define _RANGE_FIRST 0
#define _RANGE_LAST 1
class HTTPRequestInfo : public HTTPInfo{
    protected:
        // 分析GET参数
        int parseGETArgs();
    public:
        SOCKET_T socket_fd;
        int close = 0;
        string url = "/";
        string type = "GET";
        string args;
        map<string,string> GET;
        map<string,string> POST;
        int range_first = -1,range_last = -1;
    

        /* 对一段HTTP请求报文进行解析
         * 成功返回 HTTP_REQUEST_READ_SUCCESS
         * 失败返回 HTTP_REQUEST_ERROR
         */
        int parseRequest(char * request, size_t len);
        int parsePostArgs(char * data, size_t len);

        // 根据对象当前属性创建HTTP请求报文
        const char * getRequest();

        /** 获取请求的URL中文件的物理地址
         *  @param path 保存本地文件地址的string对象,若错误,则存放错误信息
         *  @return 文件存在 返回200,文件存在且只请求部分,返回206,若请求的是文件夹,返回403,文件不存在返回404
        */
        int getRequestFilePath(string &path);

        /** 获取请求头中的Range信息 
         * 
         *  @param part _RANGE_FIRST 取起始位置, _RANGE_LAST取结束位置
         *  @return Range缺省返回-1, 解析错误返回-2
         */
        int getRange(int part);
};

class HTTPFileReader : public HTTPRequestInfo{
    private:
        char dataBuffer[20480];
        size_t getLine(char * buf, size_t max_len);
        int parseHeader();
        FILE *fp = NULL;
    public:
        size_t  fileLength = 0, // 文件长度
                surplus = 0;    // 剩余未读取长度
        map<string,string> ph;  // 当前块的header属性
        string boundary;
        string filename;
        string keyname;
        int format_type = 0;    // 0:需要操作boundary 1:form-data
        HTTPFileReader();
        ~HTTPFileReader();
        HTTPFileReader(SOCKET_T fd);
        int init();
        int setReadTimeOut(unsigned int second);
        size_t read(char * buf, size_t max_len);
};

HTTPFileReader::~HTTPFileReader()
{
    if( fp!= NULL)
    {
        fclose(fp);
    }
}

int HTTPFileReader::setReadTimeOut(unsigned int second)
{
    // 设置读取超时
    #ifdef linux
    struct timeval tm = {second,0};
    #endif // linux
    #ifdef WIN32
    int tm = second*1000;
    #endif // WIN32
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (void*)&tm, sizeof(tm));
}

HTTPFileReader::HTTPFileReader()
{
    type = "GET";
}

size_t HTTPFileReader::read(char * buf, size_t max_len)
{
    size_t size = max_len > surplus ? surplus : max_len;
    if( size == 0)
    {
        if( close == 0 )
        {
            char t[128];
            getLine(t,8192);
            // fclose(fp);
            close = 1;
        }
        return 0;
    }
    int res = fread(buf, size, 1, fp);
    surplus -= size;
    if( res <= 0 )
    {
        return 0;
    }
    return size;
}

size_t HTTPFileReader::getLine(char * buf, size_t max_len)
{
    size_t l = sizeof(dataBuffer);
    bzero(dataBuffer, l);
    size_t size ;
    fgets(buf, max_len, fp);
    for (size_t i = l - 1; i > 0; i--)
    {
        if( dataBuffer[i] != 0)
        {
            return i;
        }
    }
}


/**
 * 读取分块的header,并将结果在ph中覆盖更新
 * @return 成功1 失败0
*/
int HTTPFileReader::parseHeader()
{
    ph.clear();
    size_t record = 0;
    keyname = "";
    filename = "";
    char buf[2048];
    string t;
    int flag = 0;
    int pos;

    // 干掉一行boundary和后面的\r\n
    getLine(buf, sizeof(buf));
    record += strlen(buf);
    if( strncmp(buf, boundary.c_str(), boundary.length()) )
    {
        return 0;
    }

    // 空的POST Body
    if( !strncmp(buf, (boundary + "--\r\n").c_str(), boundary.length()+4 ) )
    {
        // 设为"已关闭"状态防止read方法阻塞
        close = 1;
        return 1;
    }
    do
    {
        fgets(buf, 2048, fp);
        record += strlen(buf);
        t = buf;
        pos = t.find(':');
        if( pos == -1 ) break;
        ph[t.substr(0, pos)] = t.substr(pos + 2, t.length() - pos - 4);
        flag = 1;
    } while ( strncmp(buf,"\r\n",2) );
    record += 2;
    string cd = ph["Content-Disposition"];
    pos = cd.find("; name=\"") + 8;
    if(pos < 8) return 0;
    if( ph.count("Content-Type") )
    {
        int a = cd.rfind("\"");
        int b = cd.find("filename=\"") + 10;
        if( a == -1 || b < 10 ) return 0;
        filename = cd.substr(b , a -b);
    }
    keyname = cd.substr(pos, cd.find("\"", pos + 1) - pos );
    surplus = fileLength = atoi(header["Content-Length"].c_str()) - record - 4 - boundary.length();
    return flag;
}

/**
 * 对该HTTP POST连接进行初始化解析(包括GET解析)
 * @return 出现错误返回0,正常返回1
*/
int HTTPFileReader::init()
{
    char headerBuf[8192] ={0};
    char buffer[2048];  // 读取HTTP Header的buffer
    char buf[2];
    size_t  len,        // 一行Request Header的字符数
            total = 0;  // Request Header总字符数

    setReadTimeOut(5);
    if ( recv(socket_fd, buf, 2, MSG_WAITALL) != 2 || strncmp(buf,"T ", 2) )
    {
        return 0;
    } 
    fp = fdopen(socket_fd, "rb");
    if( !fp  )
    {
        return 0;
    }
    do
    {
        fgets(buffer, 2047, fp);
        len = strnlen(buffer, 2047);
        total += len;
        if( len >= 2047 || total > 8192 )
        {
            return 0;
        }
        strncat(headerBuf,buffer,2047);
    } while ( strncmp(buffer,"\r\n",2) );
    parseRequest(headerBuf,strlen(headerBuf));
    int pos = header["Content-Type"].find("boundary=");
    if( pos != -1 )
    {
        boundary = "--" + header["Content-Type"].substr(pos+9);
        format_type = 1;
        return parseHeader();
    }else{
        format_type = 0;
        int cl = atoi(header["Content-Length"].c_str());
        if( cl > 8192 || cl <= 0) return 0;
        char data[8192];
        if (fread(data, cl, 1, fp) < 1) return 0;
        parsePostArgs(data, cl);
        return 1;
    }
}
HTTPFileReader::HTTPFileReader(SOCKET_T fd)
{
    socket_fd = fd;
    type = "POST";
}


/**
 * 解析POST请求体
 * @param data POST请求体部分
 * @param len data长度
 * @param surplus 剩余需要读取的长度
*/
int HTTPRequestInfo::parsePostArgs(char * data, size_t len)
{
    string contenttype = header["Content-Type"];
    if( contenttype.find("boundary=") == -1 )
    {
        char *l, *p = data, *splitPos;
        int pos = 0,cnt = 0;
        size_t ll, lll;
        list<string> dataList;
        string v;
        while ( pos < len )
        {
            l = strtok(data + pos, "&");
            ll = strnlen(l, 8192);
            if( ll >= 8192){
                return HTTP_REQUEST_ERROR;
            }
            splitPos = strtok(l,"=");
            lll = strnlen(splitPos,8192);
            v = lll >= 8192 ? "" : (splitPos + lll + 1 );
            POST[splitPos] = deescapeURL(v);
            pos += ll + 1;
        }
        return 1;
    }
    
}


int HTTPRequestInfo::getRange(int part)
{
    string range = header["Range"];
    string substr;
    if( range == "" )
    {
        return -2;
    }
    int pos1,pos2;
    pos1 = range.find( part == _RANGE_FIRST ? '=' : '-' );
    pos2 = part == _RANGE_FIRST ? range.find('-') : range.length();

    if( pos1 == -1 || pos2 == -1 || pos1 == pos2 || pos1 > pos2)
    {
        return -2;
    }

    size_t len = range.length();
    substr = range.substr( pos1 + 1, pos2 - pos1 -1 ).c_str();
    return atoi(substr.c_str());
}
/**
 * 旧版本的HTTP GET解析
 * @param request HTTP请求字符串
 * @param len 请求字符串长度
*/
int HTTPRequestInfo::parseRequest(char * request, size_t len)
{
    int tag = 0,
        count = 0,
        line = 0;
    char key[1024] = {0},
        value[1024] = {0},
        *p = key;
    bool firstSpace = true;
    for (size_t i = 0; i < len && count < 1024; i++)
    {
        switch ( request[i] )
        {
        case '\r':
            break;
        case '\n':
            if (count != 0)
            {
                // 首行判断
                if( line == 0 )
                {
                    for (size_t i = 0, len = strlen(key); i < len; i++)
                    {
                        if( key[i] == ' ')
                        {
                            // 通过HTTP报文首行获取请求URL
                            key[i] = 0;
                            url = key;

                            // 解决URL编码问题
                            url = deescapeURL(url);
                            if( *url.begin() != '/' )
                                url = "/" + url;
                            int pos = url.find('?');
                            if( pos > 0)
                            {
                                args = url.substr( pos + 1 , url.length() - pos +1 );
                                url = url.substr( 0, pos );
                                parseGETArgs();
                            }

                            break;
                        }
                    }
                }else
                {
                    header[key] = value;
                }
                bzero(key,sizeof(key));
                bzero(value,sizeof(value));
                p = key;
                firstSpace = true;
                line ++;
                count = 0;
                tag = 0;
            }
            break;
        case ':':
            // k-v分割
            if( p == key )
            {
                p[count] = 0;
                p = value;
                count = 0;
                break;
            }
        case ' ':
            // 丢弃value开头的空格
            if( firstSpace && p == value)
            {
                firstSpace = false;
                break;
            }
        default:
            p[count++] = request[i];
            break;
        }
    }
    if( count >= 512 )
    {
        return HTTP_REQUEST_ERROR;
    }else
    {
        return HTTP_REQUEST_READ_SUCCESS;
    }
}

int HTTPRequestInfo::parseGETArgs()
{
    size_t len = args.length();
    char key[1024];
    char value[1024];
    char *p;
    size_t count = 0;
    p = key;
    for (size_t i = 0 ; i < len; i++)
    {
        switch (args[i])
        {
            case '=':
                p[count] = 0;
                p = value;
                count = 0;
                break;
            case '&':
                p[count] = 0;
                GET[key] = value;
                p = key;
                count = 0;
                break;
            default:
                p[count++] = args[i];
                break;
        }
    }
    p[count] = 0;
    GET[key] = value;
    return 1;
}


const char * HTTPRequestInfo::getRequest()
{
    HTTPMsg = type + " " + url + " HTTP/1.1\r\n";
    map<string,string>::iterator i = header.begin();
    for(; i != header.end() ; i++)
    {
        HTTPMsg += i->first + ": " + i->second + "\r\n";
    }
    HTTPMsg += "\r\n";
    if ( type == "POST" )
    {
        for (map<string,string>::iterator i = POST.begin(); i != POST.end(); i++)
        {
            HTTPMsg += i->first + "=" + i->second + "&";
        }
        HTTPMsg = HTTPMsg.substr(0, HTTPMsg.length() - 1);
    }
    
    return HTTPMsg.c_str();
}

int HTTPRequestInfo::getRequestFilePath(string &path)
{
    char site_root[1024] = {0};     // 站点根目录
    string tmp;
    getcwd( site_root, 1024 );
    strncat( site_root, "/web", 1024 );
    tmp = site_root;
    tmp += url;
    FILE * fp;
    int range = 0;

    range = ( header["Range"] != "" );
    // 首先判断是否为文件夹
    if ( is_dir( tmp ) )
    {
        // 判断目录下是否有默认索引index文件
        for(list<string>::iterator i = INDEX.begin(); i != INDEX.end();  i++)
        {
            fp = fopen64( (tmp + "/" + *i).c_str(), "rb" );
            if( fp )
            {
                fclose(fp);
                path = tmp + "/" + *i;
                if( range )
                {
                    return 206;
                }else{
                    return 200;
                }
            }
        }

        // 木大!403
        path = "Forbidden";
        return 403;
    }else{
        fp = fopen64( tmp.c_str(), "rb" );
        if( fp )
        {
            fclose(fp);
            path = tmp;
            if( range )
            {
                return 206;
            }else{
                return 200;
            }
        }else{
            path = "NOT FOUND";
            return 404;
        }
    }

}

