#### 数据帧定义<span id="frame"></span>
```
+---------------+---------------+---------------+---------------+---------------+
|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|
+-+-------------+---------------------------------------------------------------+
|F|             |                                                               |
|I|   opcode    |             Data length(4 Bytes)                              |
|N| (unsigned)  |                  (unsigned)                                   |
+-+-------------+---------------------------------------------------------------+
|                                                                               |
|                                   Data                                        |
|                                                                               |
+-+-------------+---------------------------------------------------------------+
```

|名称|含义|长度|
|-|-|-|
|FIN|应固定为1,用于验证数据帧是否有效,非1时,服务器收到该数据将断开连接|1bit|
|opcode|操作码|7bit|
|Data length|实体数据长度,决定需要接收多少Byte的数据|32bit|
|Data|实体数据|由Data length决定|

#### 管理器发送数据帧定义

|操作码opcode|含义|
|-|-|
|[0x0](#0x0)|控制台字符IO|
|[0x1](#0x1)|(启动task)文件传输|
|[0x2](#0x2)|(启动task)获取服务器文件列表|
|[0x3](#0x3)|服务器启停控制|


#### 管理器接收数据帧定义
|操作码opcode|含义|
|-|-|
|0x0|控制台字符IO|
|0x1|设置服务器状态为running(运行中)|
|0x2|设置服务器状态为suspended(已挂起)|


#### mc服务器对操作码的处理流程

##### 0x0<span id="0x0"></span>
将data直接写入服务器控制台

---
##### 0x1 文件传输<span id="0x1"></span>
1.解析data,以下为data样例
```
shit.jar\n              //  文件名
/mods/\n                //  在服务器上要存放的路径(相对路径)
114514\n                //  文件长度(Byte)
1562                    //  taskID
```
2.利用解析得到的`taskID`,[发起task接入请求](#task)
3.继续读取task socket,接收[文件长度]个Byte后,向task socket发送`OK`,完成,关闭task(直接close)
4.若期间出现错误,直接关闭task socket即可

----
##### 0x2 取文件列表<span id="0x2"></span>
1.解析data,以下为data样例
```
/mods                   //  目录路径(相对路径)
1562                    //  taskID
```
2.利用解析得到的taskID,[发起task接入请求](#task)
3.获取该目录下的所有文件/文件夹,构造成json格式,格式如下
```json
{
    "code":200,
    "data":[
        { "name":"config","type":"folder" },
        { "name":"mods","type":"folder" },
        { "name":"logs","type":"folder" },
        { "name":"world","type":"folder" },
        { "name":"minecraft_server.1.12.2.jar","type":"file" },
        { "name":"server.properties","type":"file" }
    ]
}
```
4.向task连接发送[数据帧](#frame),`opcode`随意,`Data length`为json字符串的长度
5.向task连接发送json字符串
6.关闭task(直接close)

---
##### 0x3 服务器启停控制<span id="0x3"></span>
1.解析data,以下为data样例
```
force-shutdown    //  控制指令
```
2.解析data得到控制指令,根据该表对服务器进行对应操作
|控制指令|操作方式|
|-|-|
|stop|向服务器控制台发送stop关闭服务器|
|force-shutdown|强制终止服务器进程|
|suspend|关闭mc服务器但不退出接入器,继续等待并处理管理器指令|
|launch|对suspend的服务器重新启动|
|reboot|对服务器stop后再启动|
---
#### task接入机制<span id="task"></span>
##### 流程
1.构造接入请求报文,报文采用这个格式`sprintf("TAK%5sE%5sE%-5d", SerID, UsrID, taskID)`
SerID:服务器实例ID
UsrID:用户ID
taskID:任务会话ID,取值范围[1-99999]
```
样例1:
TAK00001E12345E1651

样例2:
TAK    1E12345E233
```
2.向管理器发起连接,并发送接入请求报文
3.请求报文发送后,管理器会立即发送响应,响应内容为字符型的`taskID`,若该`taskID`与发出的'taskID'一致则task接入完成,接下来使用该连接按照预设的规则进行数据交换
##### 接入模拟
```
send: TAK00001E12345E1651
recv: 1651
---------OK-----------
```

##### 服务器状态码定义
```c
#define SER_STATUS_RUNNING  1                       // 服务器运行中
#define SER_STATUS_STOPPING 2                       // 服务器停止中
#define SER_STATUS_SUSPENDED    3                   // 服务器已挂起
#define SER_STATUS_LAUNCHING    4                   // 服务器启动中
#define SER_STATUS_STOPED    5                      // 服务器启动中
```