/********************************************************************************************************
* File name: http.h
* Description:  对HTTP请求数据进行具体的逻辑处理
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.28
* History: none
*********************************************************************************************************/

// 引入http功能支持模块
#include "../http/httpModule.h"

// 处理GET请求
void http_GET(baseInfo &IDInfo);
void wsRead(WebSocket *ws,Server *ser)
{
    wsHeadFrame wsFrame;
    char buffer[2048];
    int count,total,t;
    while ( ws->readHeadFrame(wsFrame) )
    {
        count = total = 0;
        while ( total < wsFrame.payload_length )
        {
            t = wsFrame.payload_length - total;
            count = ws->readData(wsFrame, buffer, t > 2048 ? 2048 : t);
            if ( count <= 0 )
            {
                return;
            }
            ser->writeSocketData(0x0, buffer, count);
            total += count;
        }

    }
}



void http_GET(baseInfo &IDInfo)
{
    HTTPRequestInfo HTTPRequest;
    HTTPResponeInfo HTTPRespone;
    char buffer[1024];

    HTTPRespone.header["Server"] = "Minecraft Server Manager Web Service";

    // 因协议识别,前3个字符被read掉了 看看什么时候有空改掉协议识别的方式
    // 消耗掉GET后面的空格
    read(IDInfo.socket, buffer ,1);
    buffer[0] = 0;
    
    // 读取后续的完整HTTP报文
    // 因为是GET请求,就不考虑请求体的内容了
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
    map<string,string>::iterator i = HTTPRequest.GET.begin();
    if(DEBUG_MODE)
    {
        for(; i != HTTPRequest.GET.end() ; i++)
        {
            cout << "key: " << i->first << " value: " << i->second << endl;
        }
        cout << "URL:" << HTTPRequest.url << " code :  " << code << endl;
    }

    switch (code)
    {
        case 200:
            HTTPRespone.outputFile(IDInfo.socket, filePath);
            break;
        case 206:
            HTTPRespone.outputFile(IDInfo.socket, filePath, HTTPRequest.getRange(_RANGE_FIRST), HTTPRequest.getRange(_RANGE_LAST));
            break;
        default:
            if ( HTTPRequest.url.substr(0,3) != "/ws" )
            {
                HTTPRespone.sendErrPage(IDInfo.socket, code, filePath);
            }
            else
            {
                if(DEBUG_MODE)
                {
                    cout << "创建WebSocket对象" << endl;
                }
                WebSocket ws(HTTPRequest, IDInfo.socket);
                if ( !ws.wsHandShake() )
                {
                    DEBUG_OUT("WebSocket握手失败");
                    return;
                }
                int SerID = atoi( ws.HTTPRequest.GET["SerID"].c_str() );
                int UsrID = atoi( ws.HTTPRequest.GET["UsrID"].c_str() );
                if ( SerID == 0 || UsrID == 0 )
                {
                    char msg[] = "Unidentified SerID or UsrID";
                    cout << "无效wsID" << endl;
                    ws.writeData(msg,strlen(msg));
                    return;
                }
                SerMutex.lock();
                if( !SerList.count(SerID) )
                {
                    SerMutex.unlock();
                    char msg[] = "Unexist Server";
                    ws.writeData(msg,strlen(msg));
                    return;
                }
                Server *ser = SerList[SerID];
                if ( ser->UsrID != UsrID )
                {
                    SerMutex.unlock();
                    char msg[] = "Authentication failed";
                    ws.writeData(msg,strlen(msg));
                    return;
                }
                if( !ser->add(&ws) )
                {
                    SerMutex.unlock();
                    char msg[] = "join server failed";
                    ws.writeData(msg,strlen(msg));
                    return;
                }
                SerMutex.unlock();
                thread wsReadTh(wsRead,&ws,ser);
                wsReadTh.join();
                if( !ws.isClose )
                {
                    ser->remove(&ws);
                    ws.close();
                    cout << "WebSocket主动退出" << endl;
                }else{
                    cout << "WebSocket被动退出" << endl;
                    ws.close();
                }
            }
            break;
    }
}
