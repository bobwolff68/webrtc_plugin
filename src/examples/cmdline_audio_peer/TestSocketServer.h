//
//  TestSocketServer.h
//  TestPeerConnectionClient
//
//  Created by Manjesh Malavalli on 10/18/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#ifndef TestPeerConnectionClient_TestSocketServer_h
#define TestPeerConnectionClient_TestSocketServer_h

#include "ThreadSafeMessageQueue.h"
#include "talk/base/thread.h"
#include "talk/base/physicalsocketserver.h"
#include "TestPeerConnectionClient.h"
#include "TestPeerConnectionObserver.h"

class TestSocketServer : public talk_base::PhysicalSocketServer
{
protected:
    talk_base::Thread* m_pThread;
    TestPeerConnectionClient* m_pClient;
    
public:
    TestSocketServer();
    virtual ~TestSocketServer();
    void SetTestPeerConnectionClient(TestPeerConnectionClient* pClient);
    virtual bool Wait(int cms, bool process_io);
};

#endif
