class HTTPResponeInfo : public HTTPInfo{
    public :
        int code = 200;
        string info = "OK";

        // 根据当前属性生成HTTP响应报文
        const char * getRespone();

        // Return the number written, or -1
        int sendHeader(int fd);
        int sendErrPage(int fd, int code, string info);

        // 向目标传输文件内容
        int outputFile(int fd, string filePath);
};



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

// 发送HTTP响应报文Header 失败返回-1或0
int HTTPResponeInfo::sendHeader(int fd)
{
    getRespone();
    return write( fd, HTTPMsg.c_str(), HTTPMsg.length() );
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


int HTTPResponeInfo::sendErrPage(int fd, int code, string info)
{
    this->code = code;
    this->info = info;
    header["Content-Type"] = "text/html";
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
    return write(fd, data.c_str(), data.length());
}
