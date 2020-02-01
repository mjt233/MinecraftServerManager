var input = document.getElementById("input")
var output = document.getElementById("output")
ws = null;
function Connect(){
    ws = new WebSocket("ws://" + location.host + "/ws?SerID=1&UsrID=12345");
    ws.onopen = function(e){
        appendText("连接成功\n")
    }
    ws.onclose = function(e){
        appendText("连接断开\n")
    }
    ws.onerror = function(e){
        appendText("连接出错\n")
    }
    ws.onmessage = function(e){
        appendText(e.data);
    }
}

function send(){
    var data = input.value + "\n";
    ws.send(data)
}

function appendText(str){
    output.value += str
    output.scrollTop = output.scrollHeight;
}

Connect();