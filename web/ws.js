function test(msg){
    var ws = new WebSocket("ws://127.0.0.1:6636/ws?SerID=1&UsrID=12345");
    ws.onopen = function(e){
        ws.send("list\n")
    }

    ws.onclose = function(e){
        console.log("onclose",e)
    }

    ws.onmessage = function(e){
        console.log(e.data);
    }
}

function testBig(){
    var ws = new WebSocket("ws://127.0.0.1:6636/ws");
    ws.onopen = function(e){
        console.log("连上了!");
        var data = "233333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333123456789"
        ws.send(data);
    }

    ws.onclose = function(e){
        console.log("onclose",e)
    }

    ws.onmessage = function(e){
        console.log("message",e.data);
    }

    ws.onerror = function(e){
        console.log("error",e);
    }
}