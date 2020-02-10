/********************************************************************************************************
* File name: websocket.h
* Description:  C++ WebSocket协议基本功能的实现
* Author: mjt233@qq.com
* Version: 1.2
* Date: 2019.2.6
* History: 
*   2019.2.6  增加die函数
*   2019.2.2  修正readHeadFrame对操作码的错误判断
* Reference0 : https://blog.csdn.net/p312011150/article/details/79758068
* Reference1 : https://blog.csdn.net/lell3538/article/details/60470558
*********************************************************************************************************/




// WebSocket数据帧定义

/*           
 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 
+-+-+-+-+-------+-+-------------+-------------------------------+
|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
|N|V|V|V|       |S|             |   (if payload len==126/127)   |
| |1|2|3|       |K|             |                               |
+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
|     Extended payload length continued, if payload len == 127  |
+ - - - - - - - - - - - - - - - +-------------------------------+
|                               |Masking-key, if MASK set to 1  |
+-------------------------------+-------------------------------+
| Masking-key (continued)       |          Payload Data         |
+-------------------------------- - - - - - - - - - - - - - - - +
:                     Payload Data continued ...                :
+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
|                     Payload Data continued ...                |
+---------------------------------------------------------------+

[FIN(1) + RSV*3(3) + opcode(4)]
[MAS(1) + Payload-len(7)]
    or add 
        2*[PayLoad len]
    or add
        8*[PayLoad len]
4*[Masking-key]

6/8/14 Byte

[Data....]

*/






#ifndef HTTP_WEB_SOCKET

#endif // !HTTP_WEB_SOCKET

#include"wsBase.h"
class WebSocket;
typedef struct wsHeadFrame{
    char opcode;
    bool FIN;
    bool mask;
    char masking_key[4];
    unsigned long long payload_length;
    unsigned long long cur_read = 0;
}wsHeadFrame;

class WebSocket{
    private:
        int fd;
    public :
        int isClose = false;
        mutex writeMutex;
        HTTPRequestInfo HTTPRequest;
        HTTPResponeInfo HTTPRespone;
        WebSocket(){};
        WebSocket(HTTPRequestInfo HTTPRequest ,int fd);
        int wsHandShake();
        string getAcceptKey(string sec_key);
        int readHeadFrame(wsHeadFrame &wsFrame);
        int readData(wsHeadFrame &wsFrame, char * buf, size_t len);
        int writeData(unsigned char opcode,const char * buf, size_t len);
        int close();
        int die(const char * msg, size_t len);

};

int WebSocket::die(const char * msg, size_t len)
{
    writeData(0x2, msg, len);
    close();
}

// Returns 0 on success, -1 for errors.
int WebSocket::close()
{
    isClose = true;
    return shutdown(fd, SHUT_RDWR);
}

WebSocket::WebSocket(HTTPRequestInfo HTTPRequest ,int fd)
{
    this->HTTPRequest = HTTPRequest;
    this->fd = fd;
    wsHeadFrame wsFrame;
    char buffer[1024];
    int count = 0;

    
}

int WebSocket::writeData(unsigned char opcode,const char * buf, size_t len)
{
    writeMutex.lock();
    unsigned char *head_frame;
    int frame_length, masking_pos;
    char payload_len;

    if( len < 126)
    {
        frame_length = 2;
        masking_pos = 2;
        payload_len = len;
    }else if(len < 65535){
        frame_length = 4;
        masking_pos = 4;
        payload_len = 126;
    }else{
        frame_length = 10;
        masking_pos = 10;
        payload_len = 127;
    }

    head_frame = (unsigned char*)malloc(frame_length);

    // 设置FIN,RSV*3和opcode
    head_frame[0] = 0x80;
    head_frame[0] += opcode;

    // 设置MASK
    head_frame[1] = 0x00;

    // 设置Payload len
    head_frame[1] += ( payload_len );
    if ( frame_length == 4 )
    {
        memcpy( head_frame + 2, &len, 2 );
        invert( (char*)head_frame + 2, 2 );
    }else if ( frame_length == 10 )
    {
        memcpy( head_frame + 2, &len, 8 );
        invert( (char*)head_frame + 2, 8 );
    }

    // // 设置masking-key
    // time_t t;
    // srand(time(&t));
    // head_frame[masking_pos] = (unsigned char)rand();
    // head_frame[masking_pos+1] = (unsigned char)rand();
    // head_frame[masking_pos+2] = (unsigned char)rand();
    // head_frame[masking_pos+3] = (unsigned char)rand();

    // // 对数据进行掩码处理
    // for (size_t i = 0; i < len; i++)
    // {
    //     buf[i] = buf[i] ^ head_frame[ masking_pos + i%4 ];
    // }
    if ( write( fd, head_frame, frame_length ) < frame_length)
    {
        free(head_frame);
        return 0;
    }
    size_t count = 0, a, i;
    while ( count < len )
    {
        a = len - count;
        i = write( fd, buf + count, ( a > 1024 ) ? 1024 : a );
        if( i <= 0 )
        {
            free(head_frame);
            return 0;
        }
        count += i;
    }
    free(head_frame);
    writeMutex.unlock();
    return count;
    
}

/** 从WebSocket读取信息,获取数据帧头信息
 * @param wsFrame 数据帧头信息 主要用来记录数据长度
 * @param buf 存放反掩码后的数据
 * @param len 缓冲区的数据长度
 * @return 返回接收的长度
 */
int WebSocket::readData(wsHeadFrame &wsFrame, char * buf, size_t len)
{
    size_t count = 0;
    unsigned long long t = wsFrame.payload_length - wsFrame.cur_read;
    size_t needRead = t > len ? len : t;
    size_t ok = 0;
    while ( ok < needRead )
    {
        count = recv( fd, buf + ok, needRead , 0);
        if( count <= 0 )
        {
            return 0;
        }
        ok += count;
    }
    
    for (size_t i = 0; i < count; i++)
    {
        buf[i] = buf[i] ^ wsFrame.masking_key[wsFrame.cur_read++%4];
    }
    return count;
    
}

// 成功1 失败0
int WebSocket::readHeadFrame(wsHeadFrame &wsFrame)
{
    unsigned char one_char;
    /* 第一个字节 */
    char FIN;
    char opcode;
    /* 第二个字节 */
    char mask;
    unsigned long long payload_len = 0;
    char mask_key[4];
    char data[1024] = {0};

    // 读取FIN与opcode
    if( recv( fd, (char *)&one_char, 1 , 0) <= 0 )
    {
        return 0;
    }
    wsFrame.FIN = ( one_char & 0x80 ) == 0x80;
    wsFrame.opcode = one_char & 0xF;
    wsFrame.cur_read = 0;
    // 读取mask开关与payload_len
    if( recv( fd, (char *)&one_char, 1 , 0) <= 0 )
    {
        return 0;
    }
    wsFrame.mask = ( one_char & 0x80 ) == 0x80;



    wsFrame.payload_length = one_char & 0x7F;
    if( wsFrame.payload_length == 126 )
    {
        // 长度升级至2Bytes
        if( recv( fd, data, 2, MSG_WAITALL ) < 2 )
        {
            return 0;
        }
        wsFrame.payload_length = 0;
        invert(data,2);
        memcpy(&wsFrame.payload_length, data, 2);
        
    }else if(wsFrame.payload_length == 127)
    {
        // 长度升级至8Bytes
        if( recv( fd, data, 8, MSG_WAITALL ) < 8 )
        {
            return 0;
        }
        invert(data,8);
        memcpy(&wsFrame.payload_length, data, 8);
    }

    if( wsFrame.mask == true )
    {    
        // 读取掩码key
        if( recv( fd, wsFrame.masking_key, 4 , MSG_WAITALL) <= 3 )
        {
            return 0;
        }
    }

    if( wsFrame.opcode != 0x1 && wsFrame.opcode !=0x0 && wsFrame.opcode != 0x2)
    {
        return 0;
    }

    return 1;

}



int WebSocket::wsHandShake()
{
    if ( HTTPRequest.header["Connection"].find("Upgrade") == -1 || HTTPRequest.header["Upgrade"] != "websocket" || HTTPRequest.header["Sec-WebSocket-Key"] == "")
    {
        if(DEBUG_MODE)
        {
            cout << "400 Bad Request" << endl;
            cout << HTTPRequest.getRequest() << endl;
        }
        HTTPRespone.sendErrPage(fd, 400, "Bad Request");
        return 0;
    }
    HTTPRespone.header.clear();
    HTTPRespone.code = 101;
    HTTPRespone.info = "Switching Protocols";
    HTTPRespone.header["Upgrade"] = "websocket";
    HTTPRespone.header["Connection"] = "Upgrade";
    HTTPRespone.header["Sec-WebSocket-Accept"] = getAcceptKey(HTTPRequest.header["Sec-WebSocket-Key"]);
    string head = HTTPRespone.getRespone(0);
    return write( fd, head.c_str(), head.length() );
}

string WebSocket::getAcceptKey(string sec_key)
{
    sec_key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    unsigned char obuf[SHA_DIGEST_LENGTH + 1] = {0};
    SHA1( (unsigned char *)sec_key.c_str(), sec_key.length(), obuf );
    return base64Encode_char( (char*)obuf, false);
}


int Server::add(WebSocket *ws)
{
    if( isClose )
    {
        return 0;
    }
    cliMutex.lock();
    WSList.push_back(ws);
    cliMutex.unlock();
    sbMutex.lock();
    char * strbuf = buffer_get_string(&sb);
    if( !strbuf )
    {
        sbMutex.unlock();
        return 1;
    }
    sbMutex.unlock();
    char * strbuf2 = (char*)malloc(strlen(strbuf)+2);
    if ( !strbuf || !strbuf2 )
    {
        cout << "malloc error" << endl;
    }
    sprintf(strbuf2, "T%s",strbuf);
    ws->writeData(0x2, strbuf2, strlen(strbuf2));
    free(strbuf);
    free(strbuf2);
    return 1;
}

// 从控制器列表中移除一个控制器
int Server::remove(WebSocket *ws)
{
    cliMutex.lock();
    list<WebSocket*>::iterator i = WSList.begin();
    for( ; i != WSList.end() ;)
    {
        if ( *i == ws )
        {
            WSList.erase(i++);
            break;
        }else
        {
            i++;
        }
    }
    cliMutex.unlock();
}

int closeWebSocket(WebSocket *ws)
{
    return ws->close();
}

void * writeWebSocket(void * arg)
{
    ThParam *tp = (ThParam*)arg;
    tp->ws->writeData(0x2, tp->msg, tp->len);
}