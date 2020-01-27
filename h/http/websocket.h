#include"wsBase.h"

// 参考文章链接 https://blog.csdn.net/p312011150/article/details/79758068

typedef struct wsHeadFrame{
    char opcode;
    bool FIN;
    bool mask;
    char masking_key[4];
    unsigned long long payload_length;
    unsigned long long cur_read = 0;
}wsHeadFrame;

class WebSocket{
    protected:
        int fd;
    public :
        HTTPRequestInfo HTTPRequest;
        HTTPResponeInfo HTTPRespone;
        WebSocket(){};
        WebSocket(HTTPRequestInfo HTTPRequest ,int fd);
        int wsHandShake();
        string getAcceptKey(string sec_key);
        int readHeadFrame(wsHeadFrame &wsFrame);
        int readData(wsHeadFrame &wsFrame, char * buf, size_t len);
        int writeData(char * buf, size_t len);
};


WebSocket::WebSocket(HTTPRequestInfo HTTPRequest ,int fd)
{
    this->HTTPRequest = HTTPRequest;
    this->fd = fd;
    wsHeadFrame wsFrame;
    char buffer[1024];
    int count = 0;

    
}

int WebSocket::writeData(char * buf, size_t len)
{
    unsigned char *head_frame;
    int frame_length, masking_pos;
    char payload_len;

    if( len < 126)
    {
        frame_length = 6;
        masking_pos = 2;
        payload_len = len;
    }else if(len < 65535){
        frame_length = 8;
        masking_pos = 4;
        payload_len = 126;
    }else{
        frame_length = 14;
        masking_pos = 10;
        payload_len = 127;
    }

    head_frame = (unsigned char*)malloc(frame_length);

    // 设置FIN,RSV*3和opcode
    head_frame[0] = 0x81;

    // 设置MASK
    head_frame[1] = 0x80;

    // 设置Payload len
    head_frame[1] += ( payload_len );
    if ( frame_length == 8 )
    {
        memcpy( head_frame + 2, &len, 2 );
        invert( (char*)head_frame + 2, 2 );
    }else if ( frame_length == 14 )
    {
        memcpy( head_frame + 2, &len, 8 );
        invert( (char*)head_frame + 2, 8 );
    }

    // 设置masking-key
    time_t t;
    srand(time(&t));
    head_frame[masking_pos] = (unsigned char)rand();
    head_frame[masking_pos+1] = (unsigned char)rand();
    head_frame[masking_pos+2] = (unsigned char)rand();
    head_frame[masking_pos+3] = (unsigned char)rand();

    // 对数据进行掩码处理
    for (size_t i = 0; i < len; i++)
    {
        buf[i] = buf[i] ^ head_frame[ masking_pos + i%4 ];
    }
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
    int count = 0;
    unsigned long long t = wsFrame.payload_length - wsFrame.cur_read;
    count = read( fd, buf, t > len ? len : t );
    if ( count <= 0 )
    {
        return 0;
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
    if( read( fd, (char *)&one_char, 1 ) <= 0 )
    {
        return 0;
    }
    wsFrame.FIN = ( one_char & 0x80 ) == 0x80;
    wsFrame.opcode = one_char & 0xF;

    // 读取mask开关与payload_len
    if( read( fd, (char *)&one_char, 1 ) <= 0 )
    {
        return 0;
    }
    wsFrame.mask = ( one_char & 0x80 ) == 0x80;



    wsFrame.payload_length = one_char & 0x7F;
    if( wsFrame.payload_length == 126 )
    {
        // 长度升级至2Bytes
        if( read( fd, data, 2 ) < 2 )
        {
            return 0;
        }
        wsFrame.payload_length = 0;
        invert(data,2);
        memcpy(&wsFrame.payload_length, data, 2);
        
    }else if(wsFrame.payload_length == 127)
    {
        // 长度升级至8Bytes
        if( read( fd, data, 8 ) < 8 )
        {
            return 0;
        }
        invert(data,8);
        memcpy(&wsFrame.payload_length, data, 8);
    }

    if( wsFrame.mask == true )
    {    
        // 读取掩码key
        if( read( fd, wsFrame.masking_key, 4 ) <= 3 )
        {
            return 0;
        }
    }
    return 1;

}




int WebSocket::wsHandShake()
{
    if ( HTTPRequest.header["Connection"].find("Upgrade") == -1 || HTTPRequest.header["Upgrade"] != "websocket" || HTTPRequest.header["Sec-WebSocket-Key"] == "")
    {
        HTTPRespone.sendErrPage(fd, 400, "Bad Request");
        cout << HTTPRequest.getRequest() << endl;
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