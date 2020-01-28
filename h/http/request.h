/********************************************************************************************************
* File name: respone.h
* Description:  HTTP模块的请求体解析
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: none
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
        int AnalysisRequest(char * request);


        // 根据对象当前属性创建HTTP请求报文
        const char * getRequest();

        // 获取请求的URL中文件的物理地址
        int getRequestFilePath(string &path);

        // 解析Range
        int getRange(int part);
};

/** 获取请求头中的Range信息 
 *  缺省返回-1
 *  错误返回-2
 * 
 *  @param part _RANGE_FIRST or _RANGE_LAST
 */
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

int HTTPRequestInfo::AnalysisRequest(char * request)
{
    size_t len = strlen(request);
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
                                url = url.substr( 0, pos - 1 );
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
    HTTPMsg = "GET " + url + " HTTP/1.0\r\n";
    map<string,string>::iterator i = header.begin();
    for(; i != header.end() ; i++)
    {
        HTTPMsg += i->first + ": " + i->second + "\r\n";
    }
    HTTPMsg += "\r\n";
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

