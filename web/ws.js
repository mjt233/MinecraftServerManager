var input = document.getElementById("input")
var output = document.getElementById("output")
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
            var line = reader.result.split(/\n/g);
            for (let index = 0; index < line.length; index++) {
                appendInfo(line[index]);
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

function byteToString(arr) {
    if (typeof arr === 'string') {
        return arr;
    }
    var str = '',
        _arr = arr;
    for (var i = 0; i < _arr.length; i++) {
        var one = _arr[i].toString(2),
            v = one.match(/^1+?(?=0)/);
        if (v && one.length == 8) {
            var bytesLength = v[0].length;
            var store = _arr[i].toString(2).slice(7 - bytesLength);
            for (var st = 1; st < bytesLength; st++) {
                store += _arr[st + i].toString(2).slice(2);
            }
            str += String.fromCharCode(parseInt(store, 2));
            i += bytesLength - 1;
        } else {
            str += String.fromCharCode(_arr[i]);
        }
    }
    return str;
}
Connect();