var target = document.getElementById("terminal");               // 终端输出DOM
var msgbox_panel = document.getElementById("msgbox-panel")      // 消息框容器

// 将字符串插入到模拟终端控制台并渲染颜色
function appendInfo(str)
{
    var p = document.createElement("p");
    var t = str.indexOf("]:");
    var l,r;
    if( t!=-1 ){
        l = str.substr(0,t + 1)
        r = str.substring(t + 1, str.length);
    }else{
        l = str
        r = ""
    }

    // append colorful string
    p.appendColStr = function(str,color,cssClass){
        str = str.replace(/\s/g,"&ensp;")
        var span = document.createElement("span")
        span.innerHTML = str;
        span.style.color = color
        if( cssClass!=undefined ){
            span.className = cssClass
        }
        this.appendChild(span)
    }

    p.appendStr = function(str, withNum){
        var res = str.match(/[0-9]+\.+[0-9]+|tps|[0-9]+%|(\.|\\|\/|\s|\:)\-?[0-9]+|WARING|WARN|ERROR|INFO|\[|\]|\:/i)
        var l,m,r;
        if( res != null ){
            l = str.substr(0,res.index).replace(/\s/g,'&ensp;')
            m = res[0]
            r = str.substring(res.index + m.length,str.length)
            // this.append(l)
            this.innerHTML += l;
            switch (m) {
                case 'INFO':
                case 'info':
                    this.appendColStr(m,"darkgreen","bold")
                    break;
                case '[':
                case ':':
                case ']':
                    this.appendColStr(m,"green")
                    break;
                case 'error':
                case 'ERROR':
                    this.appendColStr(m,"red","bold")
                    break;
                case 'warn':
                case 'WARN':
                case 'WARNING':
                case 'warning':
                    this.appendColStr(m,"yellow","bold")
                    break
                case 'tps':
                case 'TPS':
                    this.appendColStr(m,"darkseagreen","bold")
                    break
                default:
                    if( withNum == true ){
                        if(m[0] == ':'){
                            // this.append(':')
                            this.innerHTML += ':'
                            this.appendColStr(m.substr(1,m.length-1),"gray")
                        }else{
                            this.appendColStr(m,"gray")
                        }
                    }else{
                        // this.append(m)
                        this.innerHTML += m
                    }
                    break;
            }
            this.appendStr(r, withNum)
        }else{
            this.innerHTML += str.replace(/\s/g,'&ensp;')
        }
    }
    p.appendStr(l,false)
    var index = r.indexOf(": /")
    if( index != -1 ){
        p.innerHTML += r.substr(0, index+2).replace(/\s/g,"&ensp;")
        p.appendColStr(r.substring(index+2,r.length),"#99CCFF")
    }else if( index = r.indexOf(" > ") != -1){
        p.innerHTML += r.substr(0, index).replace(/\s/g,"&ensp;")
        p.appendColStr(r.substring(index,r.length),"lightblue")
    }else{
        p.appendStr(r,true)
    }
    target.appendChild(p)
    target.scrollTop = target.scrollHeight
}

function addColor(str, color)
{
    var span = document.createElement("span")
    span.innerText = str
    span.style.color = color
    return span
}

