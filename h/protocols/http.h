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

void http_test(baseInfo &IDInfo)
{
    HTTPFileReader reader(IDInfo.socket);
    reader.init();
    httpRoute(IDInfo.socket,reader);
}




void http_GET(baseInfo &IDInfo)
{
    HTTPFileReader HTTPRequest;
    HTTPResponeInfo HTTPRespone;
    char buffer[1024];
    size_t cnt;

    // 因协议识别,前3个字符被read掉了 看看什么时候有空改掉协议识别的方式
    // 消耗掉GET后面的空格
    recv(IDInfo.socket, buffer ,1, 0);
    buffer[0] = 0;
    
    // 读取后续的完整HTTP报文
    // 因为是GET请求,就不考虑请求体的内容了
    cnt = recv(IDInfo.socket, buffer , 1024, 0);

    // 解析HTTP请求头信息
    if ( HTTPRequest.parseRequest(buffer,cnt) == HTTP_REQUEST_ERROR )
    {
        HTTPRespone.sendErrPage(IDInfo.socket, 400, "Bad Request");
        return ;
    }

    httpRoute(IDInfo.socket, HTTPRequest);
}
