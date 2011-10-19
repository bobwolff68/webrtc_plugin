//
//  TestSocketServer.h
//  TestPeerConnectionClient
//
//  Created by Manjesh Malavalli on 10/18/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#ifndef TestPeerConnectionClient_TestSocketServer_h
#define TestPeerConnectionClient_TestSocketServer_h

#include <deque>
#include <string>
#include <pthread.h>
#include "talk/base/thread.h"
#include "talk/base/physicalsocketserver.h"
#include "TestPeerConnectionClient.h"

class ThreadSafeMessageQueue
{
private:
    std::deque<std::string> q;
    pthread_mutex_t qMutex;
    
public:
    ThreadSafeMessageQueue();
    ~ThreadSafeMessageQueue();
    void PostMessage(const std::string& message);
    const std::string GetNextMessage(void);
};


class TestSocketServer : public talk_base::PhysicalSocketServer
{
protected:
    talk_base::Thread* m_pThread;
    TestPeerConnectionClient* m_pClient;
    ThreadSafeMessageQueue* m_pQueue;
    
public:
    TestSocketServer(ThreadSafeMessageQueue* pQueue);
    virtual ~TestSocketServer();
    void SetTestPeerConnectionClient(TestPeerConnectionClient* pClient);
    virtual bool Wait(int cms, bool process_io);
};

#endif