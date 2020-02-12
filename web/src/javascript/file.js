var curpath = []
curpath.toString = ()=>{
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
    }else if(path != undefined){
        curpath.push(path)
    }
    console.log(curpath.toString())
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
            tb.innerHTML = '<td><a href="javascript:" onclick=\"ls(\'..\')\">返回上一级</a></td><td>-</td>'
            if( e['data'].length == 0 )
            {
                tb.innerHTML +="<tr><td>这个文件夹空空如也~~</td></tr>"
                return;
            }
            for (let index = 0; index < e['data'].length; index++) {
                const element = e['data'][index];
                tr = document.createElement("tr")
                if( element['type'] =='folder' )
                {
                    tr.innerHTML = '<td><a href=\"javascript:\" onclick=\"ls(this.innerText)\">' + element['name'] + '</a></td><td>' + element['size'] + '</td>'
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
                    tr.innerHTML = "<td>" + element['name'] + "</td><td>" + size + "</td>"
                }
                tb.appendChild(tr)
            }
        }else{
            addMsg("噔噔咚:" + e['msg'] + '( ' + curpath.toString() + ' )', 'darkred');
            curpath.pop()
        }
    })
}