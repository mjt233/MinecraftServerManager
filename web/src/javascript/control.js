function serswitch(type)
{
    var data = {
        "type":type
    }
    $.post('/api/ctl?SerID='+SerID+"&UsrID="+UsrID, data, (e)=>{
        if(e['code']!=200){
            addMsg("噔噔咚:" + e['msg'], "darkred")
        }
    })
}
