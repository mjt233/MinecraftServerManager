# Minecraft Server Manager
#### 因架构太拉跨代码太烂，项目已停止开发和维护
### 目录
* [简介](#explain)
* [特点](#feature)
* [编译](#compile)
* [其他平台](#otheros)
* [安全性](#security)
* [通信协议](#protocol)
    * [_控制器/服务器_ 接入](#serctl)
    * [HTTP(GET 和 有限制的POST)](#http)
    * [WebSocket](#websocket)(控制台回显与文件传输)

### 简介 <span id="explain"></span>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;顾名思义，这个是为了方便Minecraft腐竹管理服务器而开发的程序，通过Web服务来同时管理多个服务器。
### 使用方法 <span id="usage"></span>
1. [编译主程序](#compile)
2.  运行编译好的可执行二进制文件(manager)
3.  编译接入器
    接入器有两C++和Java版本(未开发),C++版本仅支持Linux,以下为C++版本的编译方法
    ```shell
    cd ClientDemo_cpp_version && make
    ```
    编译完成后,将编译好的可执行二进制文件(demo)和<b>同目录下的</b>Makefile移动到Minecraft服务器目录下
4.  启动接入器
    接入器用法
    ```
    ./demo [管理器地址] [管理器端口] [该服务器的实例ID] [创建者ID] "[服务器启动命令]"
    ```
    服务器实例ID与创建者ID均为数字,取值范围[1-99999]
    注意,服务器启动命令要用双引号
    接入器启动后,若成功接入管理器,Minecraft服务器也会被一同启动
5.  通过访问管理器的Web页面(默认6636端口,http协议)即可查看到接入的服务器信息


### 特点 <span id="feature"></span>
* 支持多控制端同时接入
* 支持HTTP，WebSocket协议
* 兼容所有具有控制台的服务器
* 轻量级-只有一个可执行文件
* 分布服务器集中管理（开发中）
* 内置端口映射（暂未开发）
* Web控制台（开发中）
* 同时支持外部服务器接入与本机创建服务器（暂未开发）
* 文件在线编辑（暂未开发）
### 编译 <span id="compile"></span>
```shell
$ make  
```
or
```shell
$ g++ manager.cpp -o manager -lpthread -lssl -lcrypto
```

### 其他平台 <span id="otheros"></span>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;服务端程序在开发时，没有考虑到代码的可移植性问题。对于Windows平台，主要需要更改的代码是socket，pipe管道通信部分。需要安装`pthread`与`openssl`库。
Debian系Linux下可使用以下命令安装openssl库
```shell
sudo apt install libssl-dev
```

### 安全性 <span id="security"></span>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;没有针对处理，估计会很差劲
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;若有https或域名绑定需要,使用nginx反向代理即可
### 通信协议 <span id="protocol"></span>
>默认监听6636端口
##### 服务器/控制器接入 <span id="serctl"></span>
>建立连接后，向服务器发送3个字符表示接入类型
- CTL 控制器
- SER 服务器  

后接12个字符，例如
`CTL00001E12345E` - 控制器接入实例ID为1的服务器（控制者ID：12345）
`SER00001E12345E` - 创建一个实例ID为1的服务器（创建者ID：12345）

ID有效值前面的0可以用空格代替
- 00001 表示服务器实例ID
- E ID结尾
- 12345 表示创建者/接入者ID

接入请求字符串必须为15个字符整，每个ID必须占5个字符
服务器响应“OK”即接入成功
###### 控制器或服务器接入成功后，将采用以下数据帧结构进行网络数据传输
```
+---------------+---------------+---------------+---------------+---------------+
|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|
+-+-------------+---------------------------------------------------------------+
|F|             |                                                               |
|I|   opcode    |             Data length(4 Bytes)                              |
|N|(unsigned)   |                  (unsigned)                                   |
+-+-------------+---------------------------------------------------------------+
|                                                                               |
|                           Data                                                |
|                                                                               |
+-+-------------+---------------------------------------------------------------+
```
###### 数据帧定义说明
|名称|含义|长度|
|-|-|-|
|FIN|应固定为1,用于验证数据帧是否有效,非1时,服务器收到该数据将断开连接|1bit|
|opcode|操作码|7bit|
|Data length|实体数据长度,决定需要接收多少Byte的数据|32bit|
|Data|实体数据|由Data length决定|
###### opcode操作码定义如下
|opcode|define|是否已实现|
|-|-|-|
|0x0|控制台纯文本信息|部分(来自控制器的数据)|
|0x01|启动文件传输|开发中|
|0x02|申请端口映射|否|
|0x03|连接转发|否|
##### HTTP访问 <span id="http"></span>
站点目录：程序运行目录/web
目前支持的特性
- GET请求
- GET请求参数解析
- 断点续传
>站点目录可在h/http/request.h中更改
##### WebSocket <span id="websocket"></span>
默认的Websocket请求路径`[ServerAddr]:6636/ws`
如 `ws://localhost:6636/ws`
>该路径在http.h中可更改
