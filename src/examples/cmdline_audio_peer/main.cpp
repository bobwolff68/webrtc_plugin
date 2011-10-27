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
#include "TestPeerConnectionClient.h"
#include "TestSocketServer.h"
#include "TestClientShell.h"

int main (int argc, const char * argv[])
{
    talk_base::AutoThread auto_thread;
    talk_base::Thread* thread = talk_base::Thread::Current();
    ThreadSafeMessageQueue mq;
    TestSocketServer socket_server(&mq);
    thread->set_socketserver(&socket_server);
    
    //Declare client only after set_socketserver()
    TestPeerConnectionClient testClient;
    socket_server.SetTestPeerConnectionClient(&testClient);
    
    //Create peer connection observer
    TestPeerConnectionObserver testObserver(&testClient,&mq);
    
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
