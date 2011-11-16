//
//  main.cpp
//  TestPeerConnectionClient
//
//  Created by Manjesh Malavalli on 10/14/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#include <iostream>
#include <sstream>
#include "talk/base/thread.h"
#include "WPLPeerConnectionClient.h"
#include "WPLSocketServer.h"
#include "TestClientShell.h"
#include "TestDefaults.h"

extern bool parsecmd(int argc, char**argv);

int main (int argc, const char * argv[])
{
    talk_base::AutoThread auto_thread;
    talk_base::Thread* thread = talk_base::Thread::Current();
    GoCast::ThreadSafeMessageQueue mq;
    
    if (!parsecmd(argc, (char**)argv))
    {
        std::cout << "Error parsing command line arguments." << endl;
        exit(-1);
    }
    
    GoCast::SocketServer socket_server;
    thread->set_socketserver(&socket_server);
    
    //Declare client only after set_socketserver()
    GoCast::PeerConnectionClient testClient(&mq, NULL, peername, mainserver, mainserver_port);
    socket_server.SetPeerConnectionClient(&testClient);
    
    //Run client shell
    TestClientShell shell(&mq);
    shell.startThread();
    
    //Run client
    thread->Run();
        
    //Set thread's socket server to NULL before exiting
    thread->set_socketserver(NULL);
    
    //Stop shell thread
    shell.stopThread();
    
    return 0;
}
