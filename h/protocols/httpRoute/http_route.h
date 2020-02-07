#include "wsTerminal.h"
#include "wsFileUpload.h"
/********************************************************************************************************
* File name: http_route.h
* Description:  HTTP路由模块
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.2.6
* History: none
*********************************************************************************************************/

void httpRoute(int socket_fd, HTTPRequestInfo &HTTPRequest)
{
    string filePath;
    int code = HTTPRequest.getRequestFilePath(filePath);
    HTTPResponeInfo HTTPRespone;
    switch (code)
    {
        case 200:
            HTTPRespone.outputFile(socket_fd, filePath);
            break;
        case 206:
            HTTPRespone.outputFile(socket_fd, filePath, HTTPRequest.getRange(_RANGE_FIRST), HTTPRequest.getRange(_RANGE_LAST));
            break;
        default:
            if( HTTPRequest.url.substr(0,3) == "/ws" )
            {
                terminalAccess(HTTPRequest, socket_fd);
            }else if( HTTPRequest.url.substr(0,13) == "/fileupload" ){
                wsFileUpload(HTTPRequest, socket_fd);
            }else{
                HTTPRespone.sendErrPage(socket_fd, code, filePath);
            }
            break;
    }
}
