/********************************************************************************************************
* File name: http.h
* Description:  对HTTP请求数据进行具体的逻辑处理
* Author: mjt233@qq.com
* Version: 2.0
* Date: 2019.2.6
* History: 
        *   增加简单POST支持(待完善)
        *   路由模块分离
*********************************************************************************************************/

// 引入http功能支持模块
#include "../http/httpModule.h"
#include "httpRoute/http_route.h"
// 处理GET请求
void http_GET(baseInfo &IDInfo);

// 处理POST请求
void http_post(baseInfo &IDInfo);


void http_post(baseInfo &IDInfo)
{
    HTTPRequestInfo HTTPRequest;
    HTTPResponeInfo HTTPRespone;
    char buffer[1024];
    char bodyBuffer[8192] = {0};

    int pos,        // header的长度,+4为请求体的起始位置,-1为获取失败
        cnt,        // 记录recv读取的字节数
        total,      // POST Body数据长度
        a;          // 剩余读取量
    recv(IDInfo.socket, buffer, 2, MSG_WAITALL);
    if( buffer[0] != 'T' && buffer[1] != ' ' ){
        HTTPRespone.sendErrPage(IDInfo.socket, 400, "Bad Request");
        close(IDInfo.socket);
        return;
    }
    HTTPRequest.type = "POST";

    // 读取header以及部分请求体
    cnt = recv(IDInfo.socket, buffer, 1023, 0);
    buffer[cnt] = 0;

    char *p = strstr(buffer,"\r\n\r\n");
    pos = p == NULL ? -1 : (int)(p - buffer);

    // header格式不正确
    if( pos == -1 ){
        HTTPRespone.sendErrPage(IDInfo.socket, 400, "Bad Request");
        close(IDInfo.socket);
        return;
    }
    // 分析请求头
    HTTPRequest.AnalysisRequest(buffer,pos);
    
    // 将第一次读取header时多读的POST Body数据存入到另一个buffer
    strncpy(bodyBuffer, buffer + pos + 4 , cnt - pos);
    total = atoi( HTTPRequest.header["Content-Length"].c_str());

    // 拒绝掉大于8KB的请求
    if( total >= 8192 )
    {
        HTTPRespone.sendErrPage(IDInfo.socket, 413, "Request Entity Too Large");
        return;
    }

    a = total - cnt + pos + 4;
    while ( a > 0 )
    {
        cnt = recv(IDInfo.socket, bodyBuffer + total - a, 8192, 0);
        a -= cnt;
    }


    if ( HTTPRequest.AnalysisPostBody(IDInfo.socket, bodyBuffer, total) == 0){
        HTTPRespone.sendErrPage(IDInfo.socket, 400, "Bad Request");
    }
    
    for (map<string,string>::iterator i = HTTPRequest.POST.begin(); i != HTTPRequest.POST.end(); i++)
    {
        cout << i->first << " " << i->second << endl;
    }
    

    httpRoute(IDInfo.socket, HTTPRequest);
    return;
}




void http_GET(baseInfo &IDInfo)
{
    HTTPRequestInfo HTTPRequest;
    HTTPResponeInfo HTTPRespone;
    char buffer[1024];
    size_t cnt;
    HTTPRespone.header["Server"] = "Minecraft Server Manager Web Service";

    // 因协议识别,前3个字符被read掉了 看看什么时候有空改掉协议识别的方式
    // 消耗掉GET后面的空格
    recv(IDInfo.socket, buffer ,1, 0);
    buffer[0] = 0;
    
    // 读取后续的完整HTTP报文
    // 因为是GET请求,就不考虑请求体的内容了
    cnt = recv(IDInfo.socket, buffer , 1024, 0);

    // 解析HTTP请求头信息
    if ( HTTPRequest.AnalysisRequest(buffer,cnt) == HTTP_REQUEST_ERROR )
    {
        HTTPRespone.sendErrPage(IDInfo.socket, 400, "Bad Request");
        return ;
    }

    httpRoute(IDInfo.socket, HTTPRequest);
}
