#define HTTP_REQUEST_READ_SUCCESS 2
#define HTTP_REQUEST_HEADEREND 1
#define HTTP_REQUEST_ERROR 0
class HTTPInfo{
    protected :
        string HTTPMsg;
    public:
        map<string,string> header;
};

class HTTPResponeInfo : public HTTPInfo{
    public :
        int code = 200;
        string info = "OK";
        // 根据当前属性生成HTTP响应报文
        const char * getRespone();
};


class HTTPRequestInfo : public HTTPInfo{
    public:
        string url = "/";
        string type = "GET";
        
        /* 对一段HTTP请求报文进行解析
         * 成功返回 HTTP_REQUEST_READ_SUCCESS
         * 失败返回 HTTP_REQUEST_ERROR
         */
        int AnalysisRequest(char * request);

        // 根据对象当前属性创建HTTP请求报文
        const char * getRequest();
};

const char * HTTPResponeInfo::getRespone()
{
    HTTPMsg = "HTTP/1.0 "+ to_string(code) +" "+ info + "\r\n";
    map<string,string>::iterator i = header.begin();
    for(; i != header.end() ; i++)
    {
        HTTPMsg += i->first + ": " + i->second + "\r\n";
    }
    HTTPMsg += "\r\n";
    return HTTPMsg.c_str();
}

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
                            // 获取请求URL
                            key[i] = 0;
                            url = key;
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