/********************************************************************************************************
* File name: console.h
* Description:  管理服务器的命令行控制台功能
* Author: mjt233@qq.com
* Version: 1.0
* Date: 2019.1.27
* History: none
*********************************************************************************************************/

void console_command();
void show_list();

void console_command()
{
    char input[DEFAULT_CHAR_BUFFER_SIZE];
    while (scanf("%s",input))
    {
        cout << "> Execute command: " << input << endl;
        if ( !strcmp( input, "help" ) || !strcmp( input, "?") )
        {
            cout << "******************************" << endl;
            cout << "  list:Get SerList and its CTLList" << endl;
            cout << "  exit:Close server" << endl;
            cout << "  help:Get help" << endl;
            cout << "     ?:Get help" << endl;
            cout << "******************************" << endl;
        }else if( !strcmp( input,"list" ) )
        {
            show_list();
        }else if ( !strcmp( input, "exit" ) ){
            cout << "Server closing..." << endl;
            shutdown( SER_SOCKET, SHUT_RDWR );
            exit(EXIT_SUCCESS);
        }else if ( !strcmp( input, "lock") ){
            cout << SerMutex.toString() << endl;
        }else
        {
            cout << "< Unknow command.Try use \"help\" or \"?\" to get help" << endl;
        }
        cout << endl;
        fflush(stdout);
    }
}

void show_list()
{
    if ( !SerMutex.try_lock() )
    {
        cout << "Lock SER Error " << endl;
        return;
    }
    cout << "=========Server List==========" << endl;
    for( map<int,Server*>::iterator i = SerList.begin() ; i != SerList.end() ; i++ )
    {
        cout << "SerID: " << i->second->SerID << "  UsrID: " <<  i->second->UsrID << endl;
        if ( !i->second->cliMutex.try_lock() )
        {
            cout << "Lock CTL Error " << endl;
            break;
        }
        int count=0;
        for ( list<Controller*>::iterator j = i->second->CTLList.begin()  ; j != i->second->CTLList.end() ; j++)
        {
            cout << "   CTL:" << count++ << " socket: " << (*j)->socket << endl;
        }
        i->second->cliMutex.unlock();
    }
    cout << "=========    END    ==========" << endl;
    SerMutex.unlock();
}