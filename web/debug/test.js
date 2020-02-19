id=0;
tag=0;
borwser=null;
X=null;Y=null;
doctype=null;
function view(type)
{
    var url="doc/"+type+".php";
    if(doctype!=type)
        $("#browser-iframe").attr("src",url);
    doctype=type;
    $("#browser").slideDown(500);
}
function init()
{
    borwser=document.getElementById("browser");
    iframe=$("#browser-iframe")
    document.getElementById("browser-title").onmousedown=function(e) {
        tag=1;X=e.layerX;Y=e.layerY;
    }
    document.getElementById("browser-title").onmouseup=function() {
        tag=0;
        iframe.fadeIn(100);
    }
    document.getElementsByTagName("html")[0].onmousemove=function(e){
        // console.log(e);
        if(tag==1)
        {
            iframe.fadeOut(100)
            borwser.style.top=e.clientY-Y+"px";
            borwser.style.left=e.clientX-X+"px";
        }
    }
    
}





function test(){
    var inputs=document.getElementsByTagName("form")[0];
    console.log(inputs.length)
}
function add(type=0){
    var tr=document.createElement("tr")
    var td1=document.createElement("td")
    var i1=document.createElement("input")
    td1.appendChild(i1)
    var td2=document.createElement("td")
    var i2=document.createElement("input")

    var td3=document.createElement("td");
    var  btn=document.createElement("button")
    btn.className="rm"
    btn.setAttribute("onclick","document.getElementById(\"table\").removeChild(document.getElementById("+id+"))")
    btn.type="button"
    btn.innerText="删除"
    i1.style.width="90%"
    i2.style.width="100%"
    if(type==1)
        i2.type="file";
    
    td2.appendChild(i2)
    td3.appendChild(btn);
    tr.id=String(id++)
    tr.appendChild(td1)
    tr.appendChild(td2)
    tr.appendChild(td3)
    document.getElementById("table").appendChild(tr);
}
function submit(type){
    var trs=document.getElementsByTagName("tr");
    var fd={};
    if(type==1)
    {
        for(var i=1;i<trs.length;i++)
            fd[trs[i].getElementsByTagName("td")[0].getElementsByTagName("input")[0].value]=trs[i].getElementsByTagName("td")[1].getElementsByTagName("input")[0].value
    }
    else{
        for(var i=1;i<trs.length;i++)
            trs[i].getElementsByTagName("td")[1].getElementsByTagName("input")[0].name=trs[i].getElementsByTagName("td")[0].getElementsByTagName("input")[0].value;
        
        fd=new FormData(document.getElementById("form"))
    }
    var req=$("#url").val();
        $.ajax({
            url:req,
            type:"POST",
            processData:true,
            contentType:"application/x-www-form-urlencoded",
            data:fd,
            success:function(res){
                console.log(res)
            },
            error:function(res){
                $("#respone").text("错误码:"+res.status)
            }
        })
}