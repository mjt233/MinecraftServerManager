/********************************************************************************************************
* File name: websocket.h
* Description:  使用OpenSSL库计算SHA1与BASE64编码
* Author: SDU-Leon
* Link : https://blog.csdn.net/lell3538/article/details/60470558
*********************************************************************************************************/


#ifndef HTTP_WS_BASE

#endif // !HTTP_WS_BASE

#include<openssl/sha.h>
#include<openssl/bio.h>
#include<openssl/buffer.h>
#include<openssl/evp.h>
string base64Encode(string is, bool newLine);
string base64Encode_char(char * is, bool newLine);
string toSHA1(string inputString);
void invert(char * buf, size_t len);
string base64Encode(string is, bool newLine)
{
    BIO *bmem = NULL;
    BIO *b64 = NULL;
    BUF_MEM *p;

    b64 = BIO_new(BIO_f_base64());
    if ( !newLine ){

        // 默认是每64个字符换一行,这里设置为不换行
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }


    bmem = BIO_new( BIO_s_mem() );
    b64 = BIO_push(b64, bmem);
    BIO_write( b64, is.c_str(), is.length() );
    BIO_flush( b64 );
    BIO_get_mem_ptr( b64, &p );
    BIO_set_close(b64, BIO_NOCLOSE);
    char *tmp_buffer = ( char * )malloc( p->length + 1 );
    tmp_buffer[p->length] = 0;
    memcpy(tmp_buffer, p->data, p->length);
    BIO_free_all(b64);

    string res;
    res = tmp_buffer;
    free(tmp_buffer);
    return res;
}

string toSHA1(string inputString)
{
    size_t len = inputString.length();
    unsigned char *buf = (unsigned char *)malloc( len + 1 );
    char *tmp_str = (char *)malloc( 21);
    memset(tmp_str, 0, 21);
    char tmp_ch[3] = {0};

    string res_str;

    SHA1( (const unsigned char *)inputString.c_str(), len , (unsigned char *)buf);

    for (size_t i = 0; i < 20; i++)
    {
        sprintf(tmp_ch,"%02x",buf[i]);
        strcat(tmp_str, tmp_ch);
    }
    res_str = (char *)buf;
    free(buf);
    free(tmp_str);
    return res_str;
}


// 输入二进制字符
string base64Encode_char(char * is, bool newLine)
{
    BIO *bmem = NULL;
    BIO *b64 = NULL;
    BUF_MEM *p;

    b64 = BIO_new(BIO_f_base64());
    if ( !newLine ){
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    bmem = BIO_new( BIO_s_mem() );
    b64 = BIO_push(b64, bmem);
    BIO_write( b64, is, strlen(is) );
    BIO_flush( b64 );
    BIO_get_mem_ptr( b64, &p );
    BIO_set_close(b64, BIO_NOCLOSE);
    char *tmp_buffer = ( char * )malloc( p->length + 1 );
    tmp_buffer[p->length] = 0;
    memcpy(tmp_buffer, p->data, p->length);
    BIO_free_all(b64);
    string res;
    res = tmp_buffer;
    free(tmp_buffer);
    return res;
}


void invert(char * buf, size_t len)
{
    char t;
    for (size_t i = 0; i < len/2; i++)
    {
        t = buf[i];
        buf[i] = buf[ len-i-1 ];
        buf[len-i-1] = t;
    }
    
}