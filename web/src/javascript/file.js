var curpath = []                // 用数组记录当前索引路径
var editingFileName = ''        // 当前编辑的文件名
var editingFilePath = ''        // 当前编辑的文件路径

function saveFile(){
    var args =  "SerID="+SerID+"&"+
                "UsrID="+UsrID+"&"+
                "file="+editingFileName+"&"+
                "path="+editingFilePath+"&"+
                "length="+fileEditer.getElementsByTagName("textarea")[0].value.length
    var u = "/api/fileupload?"+args;
    var fd = new FormData()
    fd.append("file",fileEditer.getElementsByTagName("textarea")[0].value);
    $.ajax({
        url:u,
        type:"POST",
        data:fd,
        processData:false,
        contentType:false,
        success:(e)=>{
            if(e['code'] == 200){
                addMsg(e['msg']+":保存成功!");
            }
        }
    })
}



curpath.toString = ()=>{        // 转为路径字符串
    let res = ''
    curpath.forEach(element => {
        res +=element + '/'
    });
    return "./"+res;
}

var filelist;

function getfile(e)
{
    var filename = e.getAttribute("name")
    var size = e.getAttribute("size");
    if(size>6291456){
        addMsg("无法预览大于6MiB的文件","darkred")
        return
    }
    var p = curpath.toString()+"/"+filename
    var data = {
        "SerID":SerID,
        "UsrID":UsrID,
        "file": p,
        "text":"1"
    }
    editingFileName = filename;
    editingFilePath = curpath.toString()
    $.get("/api/getfile", data, (e)=>{
        if( typeof(e) != 'object' ){
            fileEditer.getElementsByTagName("textarea")[0].value = e
            fileEditer.getElementsByTagName("p")[0].innerText = p
            fileEditer.classList.add("animation_show")
            fileEditer.classList.remove("hid")
            setTimeout(() => {
                fileEditer.classList.remove("animation_show")
            }, 400);
        }else{
            addMsg(e['msg'],"darkred")
        }
    })
}


function ls(path)
{
    if(path == '..'){
        curpath.pop()
    }else if(path == ' '){
        ;// 刷新
    }else if(path != undefined){
        curpath.push(path)
    }

    var args =  "SerID="+SerID+"&"+
                "UsrID="+UsrID+"&"+
                "path="+curpath.toString()
    $.get("/api/ls?"+args,(e)=>{
        console.log(e)
        if(e['code'] == 200){
            pathElem.innerText = "当前路径: " + curpath.toString()
            let tb = filelist.getElementsByTagName("tbody")[0]
            let tr
            let size
            tb.innerHTML = '<td><a class=\"iconfont\" href="javascript:" onclick=\"ls(\'..\')\">&#xe72e;返回上一级</a><td>-</td></td><td>-</td>'
            if( e['data'].length == 0 )
            {
                tb.innerHTML +="<tr><td>这个文件夹空空如也~~</td></tr>"
                return;
            }
            for (let index = 0; index < e['data'].length; index++) {
                const element = e['data'][index];
                let name = element['name']
                let fontCode;
                let descr;
                let ext
                tr = document.createElement("tr")



                if( element['type'] =='folder' )
                {
                    switch (name) {
                        case 'world':               fontCode = '&#xe62e;';descr = '存档';break;
                        case 'config':              fontCode = '&#xe61a;';descr = 'mod配置文件夹';break;
                        case 'mods':                fontCode = '&#xe61e;';descr = 'mod文件夹';break;
                        case 'logs':                fontCode = '&#xe604;';descr = '日志';break;
                        case 'crash-reports':       fontCode = '&#xe604;';descr = '崩溃报告';break;
                        case 'libraries':           fontCode = '&#xe631;';descr = '服务器核心内容';break;
                        default:                    fontCode = '&#xe604;';descr = '';break;
                    }
                    if(name.substr(0,3)=='DIM'){
                        fontCode = '&#xe62e;';
                        descr = '其他世界'
                    }
                    tr.innerHTML = '<td><a href=\"javascript:\" onclick=\"ls(\''+ name +'\')\"><span class="iconfont">' + fontCode + "</span>" + element['name'] + '</a></td><td>'+descr+'</td><td>' + element['size'] + '</td>'
                }else{
                    ext = name.split(".").pop();
                    switch (ext) {
                        case 'properties':          fontCode = '&#xe62d;';descr = '服务器配置文件夹';break;
                        case 'json':
                        case 'cfg':                 fontCode = '&#xe7bd;';descr = '';break;
                        case 'jar':                 fontCode = '&#xe624;';descr = '';break;
                        default:                    fontCode = '&#xe623;';descr = '';break;
                    }
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
                    tr.innerHTML = '<td><a onclick=\"getfile(this)\" name=\"'+ element['name'] +'\" size='+ element['size']+' href=\"javascript:\"><span class="iconfont">' + fontCode + "</span>" + element['name'] + '</a></td><td>'+descr+'</td><td>' + size + "</td>"
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
    if(uploading == 1)
    {
        addMsg("当前有正在上传的任务,请等待完成","#FFCC99")
        return;
    }
    uploading = 1;
    var args =  "SerID="+SerID+"&"+
                "UsrID="+UsrID+"&"+
                "file="+file.name+"&"+
                "path="+curpath.toString()+"&"+
                "length="+file.size
    // URL
    var u = "/api/fileupload?"+args;
    var fd = new FormData(fileForm)
    FileElem.type = "text"
    FileElem.type = "file"
    $.ajax({
        url:u,
        data:fd,
        type:"POST",
        processData:false,
        contentType:false,
        xhr: ()=>{
            Xhr = $.ajaxSettings.xhr();
            if(Xhr.upload){
                Xhr.upload.addEventListener("progress",(e)=>{
                    prog.value = (e.loaded/e.total)*100
                },false)
            }
            return Xhr;
        },
        success:(e)=>{
            uploading = 0;
            prog.value = 0
            if(e['code'] == 200)
            {
                addMsg("上传成功!");
                ls()
            }
        },error:(e)=>{
            uploading = 0;
            prog.value = 0
            addMsg("上传失败","darkred");
            console.log("ajax error",e)
        }
    })
}