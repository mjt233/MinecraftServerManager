/********************************************************************************************************
* File name: stringBuffer.h
* Description:  C语言缓冲字符串的实现
* Author: mjt233@qq.com
* Version: 2.0
* Date: 2019.1.27
* History: none
*********************************************************************************************************/
#include<stdlib.h>
#include<string.h>
// 数据结点
typedef struct buffer_node{
    struct buffer_node *next;       // 下个结点的地址
    size_t cur_len;                 // 当前结点已使用的长度
    char *data;                     // 数据
}buffer_node;

// 字符缓冲串入口
typedef struct stringBuffer{
    size_t cur_len;                 // 当前已生成的长度
    size_t max_len;                 // 最长长度
    size_t node_max_size;           // 每个结点的大小（Byte）
    struct buffer_node *head;       // 起始结点
    struct buffer_node *tail;       // 尾部结点
}stringBuffer;


struct stringBuffer buffer_create(size_t max_len, size_t node_max_size );               // 创建并初始化字符缓冲串
struct buffer_node * buffer_node_create( size_t node_data_size );                       // 创建并初始化字符缓冲串的数据结点
char * buffer_get_string( struct stringBuffer * buffer);                                // 获取字符缓冲串的字符，返回的指针需要free()释放内存
void buffer_append (stringBuffer * buffer, char * str);                                 // 向字符缓冲串中追加内容
void buffer_stat(stringBuffer * buffer);                                                // 显示字符缓冲串的信息 调试用
size_t buffer_strlen(stringBuffer buffer);                                              // 获取缓冲字符串中的字符个数
int buffer_reconstruct(stringBuffer * buffer, size_t max_len, size_t node_max_size);    // 重建一个字符缓冲串

/** 
 * 创建并初始化一个buffer
 * @param size_t max_len 数据缓冲最大结点数
 * @param size_t node_max_size 每个结点的大小（Byte）
 * @return stringBuffer 返回字符缓冲串入口结构体
 */
stringBuffer buffer_create(size_t max_len, size_t node_max_size )
{
    stringBuffer buffer;
    buffer.cur_len=0;
    buffer.max_len=max_len;
    buffer.node_max_size=node_max_size;
    buffer.head=NULL;
    buffer.tail=NULL;
    return buffer;
}

/** 
 * 创建并初始化一个字符缓冲串数据结点
 * @param size_t node_max_size 结点的大小（Byte）
 * @return buffer_node * 返回字符缓冲串数据结点指针
 */
struct buffer_node * buffer_node_create( size_t node_data_size )
{
    struct buffer_node *p;
    p = ( buffer_node *) malloc( sizeof( buffer_node ) );
    p->cur_len = 0;
    p->next = NULL;
    p->data = ( char * ) malloc( node_data_size );
    memset( p->data, 0, node_data_size );
    return p;
}


/**
 * 将数据追加到缓冲末尾
 * @param stringBuffer buffer 目标缓冲buffer
 * @param char * str 要写入的数据
 */
void buffer_append (stringBuffer * buffer, char * str)
{
    // 空缓冲初始化
    if ( buffer->tail == NULL ) {
        buffer->tail = buffer->head = buffer_node_create( buffer->node_max_size );
        buffer->cur_len = 1;
    }


    // 计算尾结点可用空间
    size_t available = buffer->node_max_size - buffer->tail->cur_len - 1;
    size_t str_len = strlen(str);


    if ( str_len  <= available ){

        // 当空间足够时
        strncat(buffer->tail->data,str, str_len );
        buffer->tail->cur_len += str_len;
    }else {

        // 空间不足时
        strncat(buffer->tail->data, str, available);

        // 更新节点的已用长度
        buffer->tail->cur_len += available;

        // 计算未追加的残余数据长度
        size_t be_left = str_len - available;
        buffer->tail->next = buffer_node_create( buffer->node_max_size );
        buffer->tail = buffer->tail->next;


        if ( ++ buffer->cur_len > buffer->max_len ){

            // 超出长度时，去头
            struct buffer_node *p = buffer->head;
            buffer->head = buffer->head->next;
            free(p->data);
            free(p);
            -- buffer->cur_len;
        }

        // 递归 继续处理剩下的字符串
        buffer_append ( buffer, str + available);
    }
}

/**
 * 获取字符缓冲串的所有数据并拼接成一个完整字符串
 * @param stringBuffer buffer 目标缓冲buffer
 * @return char * 返回拼接结果，该指针需要free()释放
 */
char * buffer_get_string( struct stringBuffer * buffer)
{
    struct buffer_node *p = buffer->head;
    if ( !p )
    {
        return NULL;
    }
    char *res;
    size_t len = ( buffer->cur_len - 1 ) * buffer->node_max_size + buffer->tail->cur_len + 1 ;
    res = ( char * )malloc( len );
    if ( !res )
    {
        return NULL;
    }
    memset( res, 0, len );
    size_t count = 0;
    while (p)
    {
        strncat( res, p->data, p->cur_len );
        count += p->cur_len;
        p = p -> next;
    }
    return res;
}

/**
 * 输出到控制台显示字符缓冲串的信息 调试用
 * @param stringBuffer buffer 目标缓冲buffer
 */
void buffer_stat(stringBuffer * buffer,int showData)
{

    buffer_node *bn = buffer->head;
    puts("-------------BufferInfo-------------");
    if ( showData )
    {
        printf("data:");
        while (bn)
        {
            printf("%s %ld ",bn->data,bn->cur_len);
            bn = bn->next;
            if(bn)
            {
                printf("| ");
            }
        }
    }
    printf("\n已生成节点数:%ld 每个节点的大小:%ld\n",buffer->cur_len,buffer->max_len);
    puts("------------------------------------");
}

/** 
 * 对已创建的字符缓冲串进行数据清空和重建
 * @param size_t max_len 数据缓冲最大结点数
 * @param size_t node_max_size 每个结点的大小（Byte）
 */
int buffer_reconstruct(stringBuffer * buffer, size_t max_len, size_t node_max_size)
{

    buffer_node *p1,*p2;
    p1 = p2 = buffer->head;
    while (p1)
    {
        p2 = p1;
        p1 = p1->next;
        free(p2);
    }
    *buffer = buffer_create(max_len, node_max_size);
    return 0;
}