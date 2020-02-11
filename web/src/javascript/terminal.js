var target = document.getElementById("terminal");
var msgbox_panel = document.getElementById("msgbox-panel")
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
    p.appendColStr = function(str,color,cssClass){
        var span = document.createElement("span")
        span.innerText = str;
        span.style.color = color
        if( cssClass!=undefined ){
            span.className = cssClass
        }
        this.appendChild(span)
    }

    p.appendStr = function(str, withNum){
        var res = str.match(/tps|[0-9]+%|(\.|\\|\/|\s|\:)\-?[0-9]+|WARING|WARN|ERROR|INFO|\[|\]|\:/i)
        var l,m,r;
        if( res != null ){
            l = str.substr(0,res.index)
            m = res[0]
            r = str.substring(res.index + m.length,str.length)
            this.append(l)
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
                            this.append(':')
                            this.appendColStr(m.substr(1,m.length-1),"gray")
                        }else{
                            this.appendColStr(m,"gray")
                        }
                    }else{
                        this.append(m)
                    }
                    break;
            }
            this.appendStr(r, withNum)
        }else{
            this.append(str)
        }
    }
    p.appendStr(l,false)
    var index = r.indexOf(": /")
    if( index != -1 ){
        p.append(r.substr(0, index+2))
        p.appendColStr(r.substring(index+2,r.length),"#99CCFF")
    }else if( index = r.indexOf(" > ") != -1){
        p.append(r.substr(0, index))
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

