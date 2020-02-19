// 连接WebSocket并进行验证
function Connect() {

    // 若已经有websocket连接,就先关掉现有的
    if( connectStatus == 1 )
    {
        console.log("qwq")
        ws.close()
    }
    SerID = document.getElementById("SerID").value
    UsrID = document.getElementById("UsrID").value

    // 连接WebScoekt并提供验证参数
    ws = new WebSocket("ws://" + location.host + "/ws?SerID="+ SerID +"&UsrID="+UsrID);
    ws.onopen = function (e) {
        connectStatus = 1;
    }
    ws.onclose = function (e) {
        if(success == 0){
            return
        }
        appendInfo("连接断开\n")
        addMsg("连接已断开","darkgrey")
        curpath.length = 0
        // 收起并隐藏控制面板
        cpSlideUp()
        controlPanel.className = 'hid'
        setTimeout(() => {
            loginPanel.className=""
        }, 250);
        connectStatus = 0;
        success = 0;
        UsrIDElem.value = ''
        SerIDElem.value = ''
        UsrIDElem.parentNode.classList.remove('active')
        SerIDElem.parentNode.classList.remove('active')
        terminal.innerHTML=''
    }
    ws.onerror = function (e) {
        addMsg("连接出错啦~~QAQ!! 再试一次吧QWQ","darkred")
        loginPanel.className=""
        connectStatus = 0;
        success = 0;
    }
    ws.onmessage = function (e) {
        var reader = new FileReader()

        // 因为服务器使用二进制传输,需要利用FileReader将二进制Blob对象转为字符串
        reader.readAsText(e.data)
        reader.onload = function(){
            if(reader.result[0]=='T')   // T开头的内容表示控制台输出内容
            {
                // 取出字符T后面的内容
                var text = reader.result.substr(1,reader.result.length-1)
                if( success == 0 )
                {
                    // 若未确认验证的情况下,收到OK表示验证成功
                    if(text == "OK")
                    {
                        addMsg("接入成功!~")
                        ls()
                        input.focus()
                        loginPanel.className="hid"
                        setTimeout(() => {
                            controlPanel.className = "hid active"
                        }, 250);
                        success = 1;
                    }else{
                        addMsg("接入失败,原因:"+text, "darkred")
                    }
                }

                // 将信息以换行符分割为多条信息并插入到控制台面板
                var line = text.split(/\n/g);
                for (let index = 0; index < line.length; index++) {
                    appendInfo(line[index]);
                }
            }else if(reader.result[0]=='S'){

                // 开头是S的数据表示服务器状态,收到这些后更新服务器状态显示并提示
                var data = JSON.parse(reader.result.substr(1,reader.result.length-1))
                addMsg("服务器状态:" + STATUS[data['status']-1])
                StatusElem.innerText = STATUS[data['status']-1] + "(0x" + data['status'] + ")"
                
            }
        }
    }
    return false;
}

function send() {
    if( connectStatus == 0 )
    {
        addMsg("未连接或连接已断开")
        return false;
    }
    var data = input.value + "\n";
    input.value = ""
    ws.send(data)
    return false;
}

// 插入一条悬浮提示
function addMsg(msg,color = "rgba(51, 180, 72, 0.719)")
{
    var p = document.createElement("p")
    var span = document.createElement("span")
    span.style.backgroundColor = color
    p.appendChild(span)
    span.innerText = msg
    msgbox_panel.insertBefore(p, msgbox_panel.childNodes[0])
    setTimeout(() => {
        p.className = "fade"
        setTimeout(() => {
            msgbox_panel.removeChild(p)
        }, 500);
    }, 5000 );
}