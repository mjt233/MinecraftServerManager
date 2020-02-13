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
        console.log(e)
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

function constructFileInfo(fileName, targetPath, fileSize){
    var res = fileName + "\n" + targetPath + "\n" + fileSize + "\n"
    return res;
}

function UploadFile(){
    FileElem.click()
}

function ExecuteUpload(file){
    var protocol = location.protocol == 'https' ? 'wss://' : 'ws://'
    var reader = new FileReader();
    var info = constructFileInfo(file.name, curpath.toString(), file.size)
    var fws = new WebSocket(protocol + location.host + '/fileupload?SerID='+SerID+'&UsrID='+UsrID)
    fws.onopen = ()=>{
        const fragment_size = 81920
        fws.send(info)
        reader.readAsArrayBuffer(file);
        reader.onload = ()=>{
            console.log("onload", reader.result)
            console.log(reader.result.byteLength)
            let parts = parseInt(reader.result.byteLength/fragment_size)
            let cur = 0;
            for (let i = 0; i < parts; i++) {
                fws.send( reader.result.slice(i*fragment_size, i*fragment_size + fragment_size) )
                cur += fragment_size
            }
            fws.send(reader.result.slice(cur, reader.result.byteLength))
        }
    }
    fws.onmessage = (e)=>{
        if(typeof(e.data) == 'string'){
            addMsg(e.data)
            console.log(e.data)
        }else{
            var rd = new FileReader()
            rd.readAsText(e.data)
            rd.onload=()=>{
                addMsg("Blob "+rd.result)
                console.log("Blob "+rd.result)
            }
        }
    }
    fws.onclose= (e)=>{
        console.log("fws close")
    }
}