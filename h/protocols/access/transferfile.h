void transferfile(baseInfo &IDInfo)
{
    char buffer[8192] = {0};
    int cnt;
    int taskID;
    mutex *mtx;
    cnt = recv(IDInfo.socket, buffer, 1024, 0);
    taskID = atoi(buffer);

    SerMutex.lock();
    if( SerList.count(IDInfo.SerID) == 0 )
    {
        cout << "服务器已退出" << endl;
        SerMutex.unlock();
        return;
    }
    Server *ser = SerList[IDInfo.SerID];
    ser->statMutex.lock();
    if( ser->taskList.count(taskID) == 0 )
    {
        cout << "不存在的taskID" << endl;
        ser->statMutex.unlock();
        SerMutex.unlock();
        return;
    }
    for (size_t i = 0; i < cnt; i++)
    {
        if( buffer[i] == ' ' )
            buffer[i] = 0;
    }
    
    // 回射taskID 让客户端确认传输开始
    if ( send( IDInfo.socket, buffer, strnlen(buffer, 8192), MSG_WAITALL ) != strnlen(buffer, 8192))
    {
        cout << "发送失败" << endl;
        return;
    }
    ser->taskList[taskID] = IDInfo.socket;
    mtx = ser->taskMutex[taskID];
    ser->statMutex.unlock();
    SerMutex.unlock();

    // 阻塞 直到文件传输完成或关闭
    mtx->lock();
    mtx->unlock();
    ser->statMutex.lock();
    ser->taskMutex.erase(taskID);
    ser->statMutex.unlock();
}