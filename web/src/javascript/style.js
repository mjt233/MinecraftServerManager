var controlPanelHeight = 100;
var FileElem
var prog;
var uploading = 0;
function init(){
    initMDBtn();
    initControlPanel();
    filelist = document.getElementById("filelist")
    FileElem = document.getElementById("file")
    prog = document.getElementById("prog")
    FileElem.addEventListener('change',()=>{
        ExecuteUpload(FileElem.files[0])
    })
    document.getElementById("SerID").value = 1
    document.getElementById("UsrID").value = 12345
    Connect()
    // ls()
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