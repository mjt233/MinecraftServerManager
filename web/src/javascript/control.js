function serswitch(type)
{
    var data = {
        "SerID":SerID,
        "UsrID":UsrID,
        "type":type
    }
    $.post('/api/ctl', data, (e)=>{
        if(e['code']!=200){
            addMsg("噔噔咚:" + e['msg'], "darkred")
        }
    })
}

function powertest()
{
    for (let i = 0; i < 8192; i++) {
        ls(' ')
    }
}