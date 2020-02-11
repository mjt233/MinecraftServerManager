/********************************************************************************************************
* File name: wsTerminal.h
* Description:  WebSocket控制台功能模块
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.2.6
* History: none
*********************************************************************************************************/


// 通过WebSocket接入服务器模拟终端
void terminalAccess(HTTPRequestInfo &HQ, int socket_fd);

// 接收WebSocket控制台发来的命令,转发给接入的对应的服务器
void terminalRead(WebSocket *ws,Server *ser);

void terminalAccess(HTTPRequestInfo &HQ, int socket_fd)
{
    WebSocket ws(HQ, socket_fd);
    if ( !ws.wsHandShake() )
    {
        DEBUG_OUT("WebSocket握手失败,SecKey:" << HQ.header["Sec-WebSocket-Key"]);
        ws.close();
        return;
    }
    int SerID = atoi( ws.HTTPRequest.GET["SerID"].c_str() );
    int UsrID = atoi( ws.HTTPRequest.GET["UsrID"].c_str() );
    if( SerID <= 0 || UsrID <= 0 )
    {
        ws.die( (char *)"T无效的ID",12);
        return;
    }
    SerMutex.lock();

    // 判断服务器目标是否存在
    if( !SerList.count(SerID) )
    {
        ws.die((char *)"T认证失败,服务器ID与创建用户ID不匹配",51);
        SerMutex.unlock();
        return;
    }
    Server *ser = SerList[SerID];

    // 需要请求者ID与服务器创建者ID需一致
    if ( ser->UsrID != UsrID )
    {
        SerMutex.unlock();
        ws.die((char *)"T认证失败,服务器ID与创建用户ID不匹配", 51);
        return;
    }
    if( !ser->add(&ws) )
    {
        SerMutex.unlock();
        ws.die((char *)"T服务器内部错误", 22);
        return;
    }
    SerMutex.unlock();
    thread readTh(terminalRead, &ws, ser);
    readTh.join();
    if( !ws.isClose )
    {
        ser->remove(&ws);
        DEBUG_OUT("WebSocket主动退出");
    }else{
        DEBUG_OUT("WebSocket主动退出");
    }
    ws.close();
    return;
}

void terminalRead(WebSocket *ws,Server *ser)
{
    wsHeadFrame wsFrame;
    char buffer[2048];
    int count,total,t;
    while ( ws->readHeadFrame(wsFrame) )
    {
        wsFrame.cur_read = 0;
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