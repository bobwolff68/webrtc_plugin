//
//  TestSocketServer.cpp
//  TestPeerConnectionClient
//
//  Created by Manjesh Malavalli on 10/18/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include "TestSocketServer.h"
#include "TestDefaults.h"

ThreadSafeMessageQueue::ThreadSafeMessageQueue()
{
    pthread_mutex_init(&qMutex, NULL);
}

ThreadSafeMessageQueue::~ThreadSafeMessageQueue()
{
    
    pthread_mutex_destroy(&qMutex);
}

void ThreadSafeMessageQueue::PostMessage(ParsedCommand& Msg)
{
    int pthreadRet = pthread_mutex_lock(&qMutex);
    ASSERT(0 == pthreadRet);
    UNUSED(pthreadRet);
    cmdQ.push_back(Msg);
    pthreadRet = pthread_mutex_unlock(&qMutex);
    ASSERT(0 == pthreadRet);
}

ThreadSafeMessageQueue::ParsedCommand ThreadSafeMessageQueue::GetNextMessage(void)
{
    ParsedCommand NextMsg;
    int pthreadRet = pthread_mutex_lock(&qMutex);
    ASSERT(0 == pthreadRet);
    UNUSED(pthreadRet);
    
    if(!cmdQ.empty())
    {
        NextMsg = cmdQ.front();
        cmdQ.pop_front();
    }
    
    pthreadRet = pthread_mutex_unlock(&qMutex);
    ASSERT(0 == pthreadRet);
    
    return NextMsg;
}

TestSocketServer::TestSocketServer(ThreadSafeMessageQueue* pQueue):
m_pQueue(pQueue)
{
    m_pThread = talk_base::Thread::Current();
    m_pClient = NULL;
}

TestSocketServer::~TestSocketServer()
{
    
}

void TestSocketServer::SetTestPeerConnectionClient(TestPeerConnectionClient *pClient)
{
    ASSERT(NULL == m_pClient);
    m_pClient = pClient;
}

bool TestSocketServer::Wait(int cms, bool process_io)
{
    ASSERT(NULL != m_pQueue);
    ASSERT(NULL != m_pThread);
    
    //Execute next command
    
    return talk_base::PhysicalSocketServer::Wait(1000, process_io);
}
