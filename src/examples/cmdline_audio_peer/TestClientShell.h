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
        std::string shellCmd;
        
        do
        {
            std::cout << GetPeerName() << "> " << std::flush;
            std::cin >> shellCmd;
            m_pQueue->PostMessage(shellCmd);
            
        } while(shellCmd != "signout");
   
        return 0;
    }
};

#endif