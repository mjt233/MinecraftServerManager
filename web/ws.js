var input = document.getElementById("input")
var output = document.getElementById("output")
var STATUS = ["运行中","停止中","已挂起","启动中","已停止"]
var ws = null;
var connectStatus = 0;
var success = 0;
var loginPanel = document.getElementById("login-panel")
var controlPanel = document.getElementById("control-panel")
var SerID,UsrID;
var StatusElem = document.getElementById("sstatus")
function Connect() {
    if( connectStatus == 1 )
    {
        console.log("qwq")
        ws.close()
    }
    SerID = document.getElementById("SerID").value
    UsrID = document.getElementById("UsrID").value
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
        loginPanel.className=""
        connectStatus = 0;
        success = 0;
    }
    ws.onerror = function (e) {
        addMsg("连接出错啦~~QAQ!! 再试一次吧QWQ","darkred")
        loginPanel.className=""
        connectStatus = 0;
        success = 0;
    }
    ws.onmessage = function (e) {
        var reader = new FileReader()
        reader.readAsText(e.data)
        reader.onload = function(){
            if(reader.result[0]=='T')
            {
                var text = reader.result.substr(1,reader.result.length-1)
                if( success == 0 )
                {
                    if(text == "OK")
                    {
                        addMsg("接入成功!~")
                        input.focus()
                        loginPanel.className="hid"
                        controlPanel.className = "hid active"
                        success = 1;
                    }else{
                        addMsg("接入失败,原因:"+text, "darkred")
                    }
                }
                var line = text.split(/\n/g);
                for (let index = 0; index < line.length; index++) {
                    appendInfo(line[index]);
                }
            }else if(reader.result[0]=='S'){
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

function addMsg(msg,color = "rgba(51, 180, 72, 0.719)")
{
    var p = document.createElement("p")
    p.innerText = msg
    p.style.backgroundColor = color
    msgbox_panel.insertBefore(p, msgbox_panel.childNodes[0])
    setTimeout(() => {
        p.className = "fade"
        setTimeout(() => {
            msgbox_panel.removeChild(p)
        }, 500);
    }, 5000 );
}