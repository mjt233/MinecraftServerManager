
class HTTPRequestInfo : public HTTPInfo{
    public:
        string url = "/";
        string type = "GET";
        
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
        int getRangeFirst();
        int getRangeLast();
};



int HTTPRequestInfo::AnalysisRequest(char * request)
{
    size_t len = strlen(request);
    int tag = 0,
        count = 0,
        line = 0;
    char key[512] = {0},
        value[512] = {0},
        *p = key;
    bool firstSpace = true;
    for (size_t i = 0; i < len && count < 512; i++)
    {
        switch ( request[i] )
        {
        case '\r':
        case '\n':
            // 连续遇到两次\r和\n表示一行结束
            if ( ++tag == 2 && count != 0)
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
                            url = deescapeURL(url);
                            if( *url.begin() != '/' )
                                url = "/" + url;
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
                return 200;
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
            return 200;
        }else{
            path = "NOT FOUND";
            return 404;
        }
    }

}
