/********************************************************************************************************
* File name: respone.h
* Description:  HTTP模块的请求体解析
* Author: mjt233@qq.com
* Version: 1.2
* Date: 2019.2.6
* History: 
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
        int AnalysisGETArgs();
    public:
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
        int AnalysisRequest(char * request, size_t len);
        int AnalysisPostBody(int sock_fd,  char * firstData, size_t firstDataLen);

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


int HTTPRequestInfo::AnalysisPostBody(int sock_fd,  char * data, size_t len)
{
    if( header["Content-Type"].find("application/x-www-form-urlencoded") != -1 ){
        char *l, *p = data, *splitPos;
        int pos = 0,cnt = 0;
        size_t ll, lll;
        list<string> dataList;
        while ( pos < len )
        {
            l = strtok(data + pos, "&");
            ll = strnlen(l, 8192);
            if( ll >= 8192){
                return HTTP_REQUEST_ERROR;
            }
            splitPos = strtok(l,"=");
            lll = strnlen(splitPos,8192);
            POST[splitPos] = lll >= 8192 ? "" : (splitPos + lll + 1 );

            pos += ll + 1;
        }
        return 1;
    }else{
        return 0;
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

int HTTPRequestInfo::AnalysisRequest(char * request, size_t len)
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
                                AnalysisGETArgs();
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

int HTTPRequestInfo::AnalysisGETArgs()
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

