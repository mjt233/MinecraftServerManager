

#include <dirent.h>
#include "urlescape.h"
#define HTTP_REQUEST_READ_SUCCESS 2
#define HTTP_REQUEST_HEADEREND 1
#define HTTP_REQUEST_ERROR 0


// 获取GMT时间
string getGMTTime();

// 获取文件拓展名
string getFileExtension(string name);
bool is_dir(string path);

// 对全局变量MIME_TYPE进行初始化
int mime_type_init();




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
        // Return the number written, or -1
        int sendHeader(int fd);
        int sendErrPage(int fd, int code, string info);
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
    header["date"] = getGMTTime();
    HTTPMsg = "HTTP/1.0 "+ to_string(code) +" "+ info + "\r\n";
    map<string,string>::iterator i = header.begin();
    for(; i != header.end() ; i++)
    {
        HTTPMsg += i->first + ": " + i->second + "\r\n";
    }
    HTTPMsg += "\r\n";
    return HTTPMsg.c_str();
}

// Return the number written, or -1
int HTTPResponeInfo::sendHeader(int fd)
{
    getRespone();
    return write( fd, HTTPMsg.c_str(), HTTPMsg.length() );
}

int HTTPResponeInfo::sendErrPage(int fd, int code, string info)
{
    this->code = code;
    this->info = info;
    header["Content-Type"] = "text/html";
    if ( sendHeader(fd) <=0 )
    {
        return 0;
    }
    string msg = to_string(code) + " " + info;
    string data = "<html>"
    "<head><title>ERROR</title></head>"
    "<body><h1>" + msg + "</h1></body>"
    "</html>"
    ;
    return write(fd, data.c_str(), data.length());
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
                            // 通过HTTP报文首行获取请求URL
                            key[i] = 0;
                            url = key;
                            url = deescapeURL(url);
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



/* 单独的函数定义 */

bool is_dir(string path)
{
    if ( opendir( path.c_str()) )
    {
        return true;
    }else
    {
        return false;
    }
}

string getFileExtension(string name)
{
    size_t i = name.length();
    for (; i > 0 && name[i] != '.'; i--);

    return i == 0 ? "" : name.substr(i+1, name.length()-i);
    
}

string getGMTTime()
{
    time_t rawTime;
    struct tm* timeInfo;
    char tmp[30]={0};
    string res;
    time(&rawTime);
    timeInfo = gmtime(&rawTime);
    strftime(tmp,sizeof(tmp),"%a, %d %b %Y %H:%M:%S GMT",timeInfo);
    res = tmp;
    
    return res;
}


int mime_type_init()
{
    // image
    MIME_TYPE["jpg"] = "image/jpeg";
    MIME_TYPE["jpeg"] = "image/jpeg";
    MIME_TYPE["gif"] = "image/gif";
    MIME_TYPE["png"] = "image/x-png";
    MIME_TYPE["ico"] = "image/x-icon";

    // text
    MIME_TYPE["json"] = "text/json";
    MIME_TYPE["html"] = "text/html";
    MIME_TYPE["htm"] = "text/html";
    MIME_TYPE["css"] = "text/css";
    MIME_TYPE["js"] = "application/x-javascript";
    MIME_TYPE["txt"] = "text/plain";
    MIME_TYPE["cpp"] = "text/plain";
    MIME_TYPE["c"] = "text/plain";
    MIME_TYPE["java"] = "text/plain";
    MIME_TYPE["php"] = "text/plain";
    MIME_TYPE["go"] = "text/plain";
    MIME_TYPE["htm"] = "text/plain";

    // media
    MIME_TYPE["mp4"] = "video/mpeg4";
    MIME_TYPE["mp3"] = "audio/mp3";
    MIME_TYPE["wav"] = "audio/wav";

    // application
    MIME_TYPE["pdf"] = "application/pdf";

    // unknow
    MIME_TYPE["__UNKNOW__"] = "application/octet-stream";
    MIME_TYPE[""] = "application/octet-stream";
    return 0;
}