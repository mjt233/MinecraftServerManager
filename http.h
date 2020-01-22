#include "h/httpModule.h"

void http_GET(baseInfo &IDInfo);

void http_GET(baseInfo &IDInfo)
{
    HTTPRequestInfo HTTPRequest;
    HTTPResponeInfo HTTPRespone;
    char buffer[1024];
    char url[512];

    // 消耗掉GET后面的空格
    read(IDInfo.socket, buffer ,1);
    buffer[0] = 0;

    // 读取后续完整HTTP报文
    read(IDInfo.socket, buffer , 1024);

    // 解析HTTP请求头信息
    HTTPRequest.AnalysisRequest(buffer);


    write( IDInfo.socket, HTTPRespone.getRespone(), strlen(HTTPRespone.getRespone()) );

}
