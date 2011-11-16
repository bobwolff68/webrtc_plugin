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
#include "WPLPeerConnectionClient.h"
#include "WPLSocketServer.h"
#include "shell.h"

bool bBreakReceived=false;

// Must have a signal handler.
//extern std::string peername;

class TestClientShell : public ThreadSingle
{
public:
    TestClientShell(GoCast::ThreadSafeMessageQueue* pQueue):
    m_pQueue(pQueue)
    {
        
    }
    
    ~TestClientShell()
    {
        
    }
    
private:
    GoCast::ThreadSafeMessageQueue* m_pQueue;
    
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
            {
                ParsedMessage cmd = cmdshell.getPairs();
                if("signin" == cmd["command"] || "SIGNIN" == cmd["command"])
                {
                    if(true == cmd["server"].empty())
                    {
                        cmd["server"] = mainserver;
                    }
                    
                    if(true == cmd["serverport"].empty())
                    {
                        cmd["serverport"] = mainserver_port;
                    }
                    
                    if(true == cmd["peername"].empty())
                    {
                        cmd["peername"] = peername;
                    }
                }
                m_pQueue->PostMessage(cmdshell.getPairs());
            }
        } while(cmd_ok && !bBreakReceived);
   
        ParsedMessage exitpairs;
        exitpairs["command"] = "QUIT";
        m_pQueue->PostMessage(exitpairs);
        
//        std::cout << "Exiting Shell Thread - workerBee()" << endl;
        return 0;
    }
};

#endif
