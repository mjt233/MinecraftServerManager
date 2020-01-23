#include "h/httpModule.h"

void http_GET(baseInfo &IDInfo);
void http_GET(baseInfo &IDInfo)
{
    HTTPRequestInfo HTTPRequest;
    HTTPResponeInfo HTTPRespone;
    char buffer[1024];

    HTTPRespone.header["Server"] = "Minecraft Server Manager Web Service";

    // 消耗掉GET后面的空格
    read(IDInfo.socket, buffer ,1);
    buffer[0] = 0;

    // 读取后续的完整HTTP报文
    read(IDInfo.socket, buffer , 1024);

    // 解析HTTP请求头信息
    if ( HTTPRequest.AnalysisRequest(buffer) == HTTP_REQUEST_ERROR )
    {
        HTTPRespone.sendErrPage(IDInfo.socket, 400, "Bad Request");
        return ;
    }

    // 存放文件完整路径
    string filePath;

    // Content-Type
    string content_type;
    FILE *fp = NULL;
    int code = HTTPRequest.getRequestFilePath(filePath);
    int count = 0;
    HTTPRespone.header["Connection"] = "close";
    switch (code)
    {
        case 200:
            HTTPRespone.outputFile(IDInfo.socket, filePath);
            break;
        
        default:
            HTTPRespone.sendErrPage(IDInfo.socket, code, filePath);
            break;
    }
}
