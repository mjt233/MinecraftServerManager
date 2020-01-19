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
            exit(EXIT_SUCCESS);
        }else
        {
            cout << "< Unknow command.Try use \"help\" or \"?\" to get help" << endl;
        }
        cout << endl << "$ ";
        fflush(stdout);
    }
}

void show_list()
{
    pthread_mutex_lock(&SerMutex);
    cout << "=========Server List==========" << endl;
    for( map<int,Server*>::iterator i = SerList.begin() ; i != SerList.end() ; i++ )
    {
        cout << "SerID: " << i->second->SerID << "  UsrID: " <<  i->second->UsrID << endl;
        pthread_mutex_lock(&i->second->ctlMutex);
        for ( list<Controller>::iterator j = i->second->CTLList.begin()  ; j != i->second->CTLList.end() ; j++)
        {
            cout << "   CTL:" << endl;
        }
        pthread_mutex_unlock(&i->second->ctlMutex);
    }
    cout << "=========    END    ==========" << endl;
    pthread_mutex_unlock(&SerMutex);
}