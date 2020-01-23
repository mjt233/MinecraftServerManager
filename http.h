#include "h/httpModule.h"

void http_GET(baseInfo &IDInfo);
void http_GET(baseInfo &IDInfo)
{
    HTTPRequestInfo HTTPRequest;
    HTTPResponeInfo HTTPRespone;
    char buffer[1024];
    char url[512];

    HTTPRespone.header["Content-Type"]="image/jpeg";
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

    // 开始准备读取文件

        // 获取目标文件完整路径
    char site_root[1024] = {0};     // 站点根目录
    getcwd( site_root, 1024 );
    strcat( site_root,"/web/" );
    string rq_url = site_root;
    rq_url += HTTPRequest.url == "/" ? "index.html" : *(HTTPRequest.url.rbegin()) == '/' ? "index.html": HTTPRequest.url ;

    // 根据路径中的拓展名设置对应的Content-Type
    HTTPRespone.header["Content-Type"] = ( MIME_TYPE[getFileExtension(rq_url)] == "" ? MIME_TYPE["__UNKNOW__"] : MIME_TYPE[getFileExtension(rq_url)] ) + ";charset=utf-8";
    if( is_dir( rq_url ) )
    {
        HTTPRespone.sendErrPage(IDInfo.socket, 403, "Forbidden");
        return ;
    }


    FILE *fp = fopen64( rq_url.c_str(),"rb" );
    if( !fp )
    {
        HTTPRespone.sendErrPage(IDInfo.socket, 404, "NOT FOUND");
        return ;
    }
    
    // 获取并设置文件长度
    fseek(fp, 0, SEEK_END);
    HTTPRespone.header["Content-Length"] = to_string(ftell(fp));
    fseek(fp, 0, SEEK_SET);


    // 发送HTTP响应头
    if ( write( IDInfo.socket, HTTPRespone.getRespone(), strlen(HTTPRespone.getRespone()) ) <= 0 )
    {
        return ;
    }
    int l = 0;
    while ( !feof( fp ) )
    {
        l = fread( buffer, 1, 1024, fp );
        if ( write( IDInfo.socket, buffer, l ) <= 0)
        {
            break;
        }
    }
    fclose(fp);
    
}
