var curpath = []                // 用数组记录当前索引路径
curpath.toString = ()=>{        // 转为路径字符串
    let res = ''
    curpath.forEach(element => {
        res +=element + '/'
    });
    return res;
}

var filelist;
function ls(path)
{
    if(path == '..'){
        curpath.pop()
    }else if(path == ' '){
        ;// 刷新
    }else if(path != undefined){
        curpath.push(path)
    }
    var data = {
        "SerID":SerID,
        "UsrID":UsrID,
        "path":curpath.toString()
    }

    $.post("/api/ls",data,(e)=>{
        if(e['code'] == 200){
            pathElem.innerText = "当前路径: " + curpath.toString()
            let tb = filelist.getElementsByTagName("tbody")[0]
            let tr
            let size
            tb.innerHTML = '<td><a class=\"iconfont\" href="javascript:" onclick=\"ls(\'..\')\">&#xe72e;返回上一级</a></td><td>-</td>'
            if( e['data'].length == 0 )
            {
                tb.innerHTML +="<tr><td>这个文件夹空空如也~~</td></tr>"
                return;
            }
            for (let index = 0; index < e['data'].length; index++) {
                const element = e['data'][index];
                let name = element['name']
                let fontCode;
                tr = document.createElement("tr")

                switch (name) {
                    case 'server.properties':   fontCode = '&#xe62d;';break;
                    case 'world':               fontCode = '&#xe62e;';break;
                    case 'confog':              fontCode = '&#xe61a;';break;
                    case 'mods':                fontCode = '&#xe61e;';break;
                    default:
                        if( element['type'] == 'folder' ){
                            fontCode = '&#xe604;'
                        }else{
                            fontCode = '&#xe623;'
                        }
                        break;
                }

                if( element['type'] =='folder' )
                {
                    tr.innerHTML = '<td><a href=\"javascript:\" onclick=\"ls(\''+ name +'\')\"><span class="iconfont">' + fontCode + "</span>" + element['name'] + '</a></td><td>' + element['size'] + '</td>'
                }else{
                    size = element['size']
                    if( size < 1024 ){
                        size += 'Byte'
                    }else if( size > 1024 && size < 1024000){
                        size = (size/1024).toFixed(1) + 'KiB'
                    }else if( size > 1048576 && size < 1073741824 ){
                        size = (size/1048576).toFixed(1) + 'MiB'
                    }else{
                        size = (size/1073741824).toFixed(1) + 'GiB'
                    }
                    tr.innerHTML = '<td><a href=\"javascript:\"><span class="iconfont">' + fontCode + "</span>" + element['name'] + "</a></td><td>" + size + "</td>"
                }
                tb.appendChild(tr)
            }
        }else{
            addMsg("ERROR:" + e['msg'] + '( ' + curpath.toString() + ' )', 'darkred');
            curpath.pop()
        }
    })
}

function uploadFile(){
    var fileInput = document.getElementById("file")
    var reader = new FileReader()
    fileInput.addEventListener("change", (e)=>{
        let fname,flength,path,headMsg,success = 0
        reader.readAsArrayBuffer(fileInput.files[0])
        fname = fileInput.files[0].name
        flength = fileInput.files[0].size
        path = curpath.toString()
        headMsg = fname + '\n'+ path + '\n' + flength;
        reader.onload = (e)=>{
            var fws = new WebSocket("ws://"+location.host+"/fileupload?SerID="+SerID+"&UsrID="+UsrID)
            fws.onerror = ()=>{
                addMsg("文件传输出错", "darkred")
            }
            fws.onopen = ()=>{
                fws.send(headMsg)
                fws.send(reader.result)
            }
            fws.onmessage = (e)=>{
                if( typeof(e) == 'object' ){
                    console.log("是object")
                }
                if ( typeof(e.data) != "string"){
                    addMsg("上传失败,原因: " + e.data, "darkred")
                }else{
                    addMsg("上传成功")
                }
                ls(' ')
            }
            fws.onclose = (e)=>{
                console.log("close")
            }
            console.log(reader.result)
        }
    })
    fileInput.click()
}