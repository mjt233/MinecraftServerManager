var input = document.getElementById("input")                        // 命令输入框
var terminal = document.getElementById("terminal")                  // 控制台输出
var STATUS = ["运行中","停止中","已挂起","启动中","已停止"]              // 服务器状态码对应表
var ws = null;                                                      // WebSocket对象
var connectStatus = 0;                                              // WebSocket连接状态 1:已连接 0:为连接
var success = 0;                                                    // WebSocket验证状态 1:已验证并接入 0:未验证和接入成功
var loginPanel = document.getElementById("login-panel")             // 登录输入框
var controlPanel = document.getElementById("control-panel")         // 控制面板
var SerID,UsrID;                                                    // 顾名思义
var SerIDElem = document.getElementById("SerID")
var UsrIDElem = document.getElementById("UsrID")
var StatusElem = document.getElementById("sstatus")                 // 服务器状态显示DOM(在控制面板的运行控制内)
var pathElem = document.getElementById("path")                      // 当前路径显示DOM(在控制面板的文件管理内)
var fileForm = document.getElementById("fileform");                 // 文件表单
var fileEditer = document.getElementById("fileEditer")              // 文件编辑器
var close_btn = document.getElementById("close-btn")
var maxsize_btn = document.getElementById("maxsize-btn")
var minsize_btn = document.getElementById("min-btn")
var controlPanelHeight = 100;
var FileElem
var prog;
var uploading = 0;
var history_cmd = []   //  历史命令
var history_cur = 0;            //  当前历史命令位置
initMDBtn();
initControlPanel();
filelist = document.getElementById("filelist")
FileElem = document.getElementById("file")
prog = document.getElementById("prog")
filelist.oncontextmenu = ()=>{return false}
filelist.onmousedown = (e)=>{
    if(e.which == 3 && e.toElement.tagName == 'A'){
        let elem = e.toElement
        let name = elem.getAttribute("name")
        console.log(name)
    }
}
FileElem.addEventListener('change',()=>{
    ExecuteUpload(FileElem.files[0])
})

close_btn.addEventListener("click", ()=>{
    fileEditer.classList.add("animation_close")
    setTimeout(() => {
        fileEditer.classList.add("hid")
        fileEditer.classList.remove("animation_close")
    }, 240);
})

input.addEventListener("keydown",(e)=>{
    console.log(history_cmd);
    
    var key = e.key
    var len = history_cmd.length
    if( key == "ArrowUp" ){
        history_cur--;
        if( history_cur < 0 ){
            history_cur = len;
            return
        }
    }else if( key == "ArrowDown"){
        history_cur++;
        if( history_cur >= len){
            history_cur = len;
            input.value = ''
            return
        }
    }else{
        return
    }
    input.value = history_cmd[history_cur]
})

maxsize_btn.addEventListener("click",beMax)

$("#fileEditer").draggable({
    start:()=>{
        if(fileEditer.classList.contains("maxsize")){
            beMax(0.1)
        }
    }
});

fileEditer.addEventListener("dblclick",beMax)

function beMax(delay){
    if(typeof(delay) == "number"){
        fileEditer.style.transition = "all " + typeof(delay) == "number" ? delay : "0.4" + "s";
    }else{
        fileEditer.style.transition = "all 0.4s";
    }
    if(fileEditer.classList.contains("maxsize")){
        fileEditer.style.top = fileEditer.getAttribute("top")
        fileEditer.style.left = fileEditer.getAttribute("left")
        fileEditer.classList.remove("maxsize")
    }else{
        fileEditer.setAttribute("left", fileEditer.style.left)
        fileEditer.setAttribute("top", fileEditer.style.top)
        fileEditer.style.top = ""
        fileEditer.style.left = ""
        fileEditer.classList.add("maxsize")
    }
    setTimeout(() => {
        fileEditer.style.transition = "";
    }, (typeof(delay) == "number" ? delay : "0.4")*1000);
}

function initMDBtn()
{
    var btns = document.getElementsByClassName("md")
    for (let index = 0; index < btns.length; index++) {
        const element = btns[index];
        element.addEventListener("click",()=>{
            element.classList.remove("press")
            // element.classList.add("press")
            setTimeout(() => {
                element.classList.add("press")
            }, 10);
        })
    }
}

// 初始化控制面板的索引列表动作
function initControlPanel()
{
    var nav = document.getElementById("control-panel").getElementsByTagName("nav")[0]
    var ls = nav.getElementsByTagName("li")
    for (let index = 0; index < ls.length - 1; index++) {
        const elem = ls[index];
        elem.addEventListener("click",()=>{
            let i;
            for (let index = 0; index < ls.length; index++) {
                const elem2 = ls[index];
                elem2.classList.remove("active")
                if(ls[index] == elem)
                {
                    i = index
                }
            }
            document.getElementById("panel-container").style.top = -(i*controlPanelHeight)+"%"
            setTimeout(() => {
                elem.classList.add("active")

            }, 10);
        })
    }
}


// 控制面板的收缩
function cpSlideUp()
{
    var cp = document.getElementById("control-panel");
    var btn = cp.getElementsByClassName("handle")[0]
    if(cp.classList.contains("hid"))
    {
        cp.classList.remove("hid")
        btn.innerText = "收回"
    }else{
        cp.classList.add("hid")
        btn.innerText = "控制面板"
    }
}