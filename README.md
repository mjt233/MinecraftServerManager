# Minecraft Server Manager
---  
### 目录
* [简介](#explain)
* [特点](#feature)
* [编译](#compile)
* [其他平台](#otheros)
* [安全性](#security)
* [通信协议](#protocol)
    * [_控制器/服务器_ 接入](#serctl)
    * _快捷指令_ (未完成)
    * [HTTP(GET only)](#http)
    * [WebSocket](#websocket)

### 简介 <span id="explain"></span>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;顾名思义，这个是为了方便Minecraft腐竹管理服务器而开发的程序，通过Web服务来同时管理多个服务器。
### 特点 <span id="feature"></span>
* 支持多控制端同时接入
* 支持HTTP，WebSocket协议
* 兼容所有具有控制台的服务器
* 轻量级-只有一个可执行文件
* 分布服务器集中管理（暂未开发）
* 内置端口映射（暂未开发）
* Web控制台（暂未开发）
* 同时支持外部服务器接入与本机创建服务器（暂未开发）
* 文件在线编辑（暂未开发）
### 编译 <span id="compile"></span>
###### &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;在64位 Linux平台下使用命令行(目前仅支持Linux)
```shell
$ make  
```
or
```shell
$ g++ manager.cpp -o manager -lpthread -lssl -lcrypto
```

### 其他平台 <span id="otheros"></span>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;服务端程序在开发时，没有考虑到代码的可移植性问题。对于Windows平台，主要需要更改的代码是socket，pipe管道通信部分。需要安装`pthread`与`openssl`库。

### 安全性 <span id="security"></span>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;没有针对处理，估计会很差劲

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