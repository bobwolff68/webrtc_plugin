//
//  TestClientShell.h
//  TestPeerConnectionClient
//
//  Created by Manjesh Malavalli on 10/14/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#ifndef TestPeerConnectionClient_TestClientShell_h
#define TestPeerConnectionClient_TestClientShell_h

#include <iostream>
#include "ThreadSingle.h"
#include "TestDefaults.h"
#include "TestPeerConnectionClient.h"
#include "TestSocketServer.h"
#include "shell.h"

bool bBreakReceived=false;

// Must have a signal handler.
extern std::string peername;

class TestClientShell : public ThreadSingle
{
public:
    TestClientShell(ThreadSafeMessageQueue* pQueue):
    m_pQueue(pQueue)
    {
        
    }
    
    ~TestClientShell()
    {
        
    }
    
private:
    ThreadSafeMessageQueue* m_pQueue;
    
private:
    int workerBee(void)
    {
        //Run command shell
        Shell cmdshell;
        bool cmd_ok=true;
        
        do
        {
            std::cout << "AudioClientTest::" << peername << " > " << std::flush;
            cmd_ok = cmdshell.parseLine(cin, false);
            if (cmd_ok) 
                m_pQueue->PostMessage(cmdshell.getPairs());
        } while(cmd_ok && !bBreakReceived);
   
        map<std::string, std::string> exitpairs;
        exitpairs["command"] = "QUIT";
        m_pQueue->PostMessage(exitpairs);
        
//        std::cout << "Exiting Shell Thread - workerBee()" << endl;
        return 0;
    }
};

#endif
