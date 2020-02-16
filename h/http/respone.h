/********************************************************************************************************
* File name: respone.h
* Description:  HTTP模块的响应体功能实现
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: none
*********************************************************************************************************/



#ifndef HTTP_RESPONE

#endif // !HTTP_RESPONE
class HTTPResponeInfo : public HTTPInfo{
    public :
        int code = 200;
        string info = "OK";

        // 根据当前属性生成HTTP响应报文
        const char * getRespone();
        const char * getRespone(int autoAddContentType);
        HTTPResponeInfo()
        {
            header["Cache-Control"] = "no-cache";
            header["Accept-Ranges"] = "bytes";
            header["Connection"] = "close";
            header["Server"] = "Minecraft Server Manager Web Service";
        }
        // Return the number written, or -1
        int sendHeader(int fd);
        int sendErrPage(int fd, int code, string info);

        // 向目标传输文件内容
        int outputFile(int fd, string filePath);
        int outputFile(int fd, string filePath, int range_first, int range_right);
        int sendJsonMsg(SOCKET_T fd, int httpCode, int statCode, const char *headerInfo, const char* msg);
        void setJsonHeader();
};


void HTTPResponeInfo::setJsonHeader()
{
    header["Content-Type"] = "application/json";
}


// 生成HTTP响应报文header
const char * HTTPResponeInfo::getRespone()
{
    header["date"] = getGMTTime();
    // header["Connection"] = "close";
    HTTPMsg = "HTTP/1.1 "+ to_string(code) +" "+ info + "\r\n";
    map<string,string>::iterator i = header.begin();
    header["Content-Type"] = header["Content-Type"] == "" ? "text/html" : header["Content-Type"] ;
    for(; i != header.end() ; i++)
    {
        HTTPMsg += i->first + ": " + i->second + "\r\n";
    }
    HTTPMsg += "\r\n";
    return HTTPMsg.c_str();
}

/**
 * 发送JSON格式响应并关闭连接
 * @param fd SOCKET描述符
 * @param httpCode HTTP响应码
 * @param headerInfo HTTP响应状态消息
 * @param statCode  JSON结果状态码
 * @param msg JSON消息
*/
int HTTPResponeInfo::sendJsonMsg(SOCKET_T fd, int httpCode, int statCode, const char *headerInfo, const char* msg)
{
    code = httpCode;
    info = headerInfo;
    char json[2048];
    snprintf(json, 2048, "{\"code\":%d,\"msg\":\"%s\"}", statCode, msg);
    header["Content-Type"] = "application/json";
    header["Content-Length"] = to_string(strlen(json));
    sendHeader(fd);
    send(fd,json,strlen(json), MSG_WAITALL);
}

// 生成HTTP响应报文header
const char * HTTPResponeInfo::getRespone( int autoAddContentType )
{
    HTTPMsg = "HTTP/1.1 "+ to_string(code) +" "+ info + "\r\n";
    map<string,string>::iterator i = header.begin();
    for(; i != header.end() ; i++)
    {
        HTTPMsg += i->first + ": " + i->second + "\r\n";
    }
    HTTPMsg += "\r\n";
    return HTTPMsg.c_str();
}

/**
 *  发送HTTP报文头部
 *  @return 失败返回-1 or 0 成功返回正整数
*/
int HTTPResponeInfo::sendHeader(int fd)
{
    int a;
    getRespone();
    try
    {
        a = send( fd, HTTPMsg.c_str(), HTTPMsg.length() , MSG_WAITALL);
    }
    catch(...)
    {
        cout << "发送header失败" << endl;
    }
    
    return (a == HTTPMsg.length());
}

// 向目标输出文件 成功返回1
int HTTPResponeInfo::outputFile(int fd, string filePath)
{

    FILE * fp;
    string content_type;
    if ( !(fp = fopen( filePath.c_str(), "rb" )) )
    {
        sendErrPage(fd, 500, "Internal Server Error");
        return -1;
    }
    content_type = MIME_TYPE[getFileExtension(filePath)] + ";charset=UTF-8";
    header["Content-Type"] = content_type == "" ? MIME_TYPE["__UNKNOW__"] : content_type;
    fseek( fp, 0, SEEK_END );
    header["Content-Length"] = to_string( ftell(fp) );
    fseek( fp, 0, SEEK_SET );
    header["Last-Modified"] = getLastModified(filePath);
    if (sendHeader(fd) <= 0)
    {
        return -1;
    }
    int count = 0;
    char buffer[4096];
    int success = 1;
    while ( !feof(fp) )
    {
        count = fread( buffer, 1, 4096, fp );
        if( count <= 0 || write( fd, buffer, count ) <= 0 )
        {
            success = 0;
            break;
        }
    }
    fclose(fp);
    return success;
}

int HTTPResponeInfo::outputFile(int fd, string filePath, int range_first, int range_last)
{
    FILE * fp;
    string content_type;
    size_t content_length;    // 需要传输的文件大小
    if ( !(fp = fopen( filePath.c_str(), "rb" )) )
    {
        sendErrPage(fd, 500, "Internal Server Error");
        return -1;
    }
    content_type = MIME_TYPE[getFileExtension(filePath)] + ";charset=UTF-8";
    header["Content-Type"] = content_type == "" ? MIME_TYPE["__UNKNOW__"] : content_type;

    // 失败或缺省当做从头开始
    if (range_first < 0)
    {
        range_first = 0;
    }
    
    // 指针定位到文件末尾以计算文件长度
    fseek( fp, 0, SEEK_END );

    if( range_last > 0 )
    {
        // 计算从起始到结束需要读取多少字节
        content_length = range_last - range_first + 1;
    }else
    {
        // 失败或缺省当做到文件结尾结束
        content_length = ftell(fp) - range_first;
    }
    
    // 防止超出文件范围
    if( content_length > ftell(fp) )
    {
        content_length = ftell(fp);
    }

    header["Content-Length"] = to_string( content_length );

    
    header["Content-Range"] = " bytes " + \
                to_string(range_first) +"-"+ \
                to_string( range_last <= 0 ? ( content_length -1 ): range_last ) + "/" + \
                to_string( ftell(fp) );
    header["Last-Modified"] = getLastModified(filePath);
    code = 206;
    info = "Partial Content";
    if (sendHeader(fd) <= 0)
    {
        return -1;
    }
    size_t count = 0;
    char buffer[1024];
    int success = 1;

    // 文件流指针定位到Range起始位置
    fseek( fp, range_first, SEEK_SET );
    while ( !feof(fp) )
    {

        // 每次最多读1024
        count = fread( buffer, 1, content_length > 1024 ? 1024 : content_length, fp );

        // 这里的content_length将作为 剩余需要读取的字节数
        content_length -= count;

        // 文件读取或socket发送出错时
        if( count <= 0 || send( fd, buffer, count, 0 ) <= 0 )
        {
            success = 0;
            break;
        }
    }
    fclose(fp);
    return success;
}

int HTTPResponeInfo::sendErrPage(int fd, int code, string info)
{
    this->code = code;
    this->info = info;
    header["Content-Type"] = "text/html;charset=UTF-8";
    header["Connection"] = "close";
    string msg = to_string(code) + " " + info;
    string data = "<html>"
    "<head><title>ERROR</title></head>"
    "<body><h1>" + msg + "</h1></body>"
    "</html>"
    ;
    header["Content-Length"] = to_string( data.length() );
    if ( sendHeader(fd) <=0 )
    {
        return 0;
    }
    return send(fd, data.c_str(), data.length(), 0);
}
