var input = document.getElementById("input")
var output = document.getElementById("output")
var STATUS = ["运行中","停止中","已挂起","启动中","已停止"]
ws = null;
function Connect() {
    ws = new WebSocket("ws://" + location.host + "/ws?SerID=1&UsrID=12345");
    ws.onopen = function (e) {
        appendInfo("连接成功\n")
    }
    ws.onclose = function (e) {
        appendInfo("连接断开\n")
    }
    ws.onerror = function (e) {
        appendInfo("连接出错\n")
    }
    ws.onmessage = function (e) {
        var reader = new FileReader()
        reader.readAsText(e.data)
        reader.onload = function(){
        //     var line = reader.result.split(/\n/g);
        //     for (let index = 0; index < line.length; index++) {
        //         appendInfo(line[index]);
        //     }
            if(reader.result[0]=='T')
            {
                var line = reader.result.substr(1,reader.result.length-1).split(/\n/g);
                for (let index = 0; index < line.length; index++) {
                    appendInfo(line[index]);
                }
            }else if(reader.result[0]=='S'){
                var data = JSON.parse(reader.result.substr(1,reader.result.length-1))
                console.log("服务器状态变化为:" + STATUS[data['status']-1])
            }
        }
    }
}

function send() {
    var data = input.value + "\n";
    input.value = ""
    ws.send(data)
    return false;
}
Connect();