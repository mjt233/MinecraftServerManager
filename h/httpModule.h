
map<string,string> MIME_TYPE;
list<string> INDEX;
#include <dirent.h>
#include "urlescape.h"

#define HTTP_REQUEST_READ_SUCCESS 2
#define HTTP_REQUEST_HEADEREND 1
#define HTTP_REQUEST_ERROR 0


class HTTPInfo{
    protected :
        string HTTPMsg;
    public:
        map<string,string> header;
};


#include "http/request.h"
#include "http/respone.h"

// 获取GMT时间
string getGMTTime();

// 获取文件拓展名
string getFileExtension(string name);
bool is_dir(string path);

// 对全局变量MIME_TYPE进行初始化
int HTTP_attr_init();




/* 独立功能模块 */

bool is_dir(string path)
{
    DIR *dir = opendir( path.c_str());
    closedir(dir);
    if ( dir )
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

// 初始化MIME类型和index文件
int HTTP_attr_init()
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

    // index
    INDEX.push_back("index.html");
    INDEX.push_back("index.htm");
    return 0;
}