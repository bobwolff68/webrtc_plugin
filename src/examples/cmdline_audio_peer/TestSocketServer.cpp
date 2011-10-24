//
//  TestSocketServer.cpp
//  TestPeerConnectionClient
//
//  Created by Manjesh Malavalli on 10/18/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#include <iostream>
#include "TestSocketServer.h"
#include "TestDefaults.h"

ThreadSafeMessageQueue::ThreadSafeMessageQueue()
{
    pthread_mutex_init(&qMutex, NULL);
}

ThreadSafeMessageQueue::~ThreadSafeMessageQueue()
{
    q.clear();
    pthread_mutex_destroy(&qMutex);
}

void ThreadSafeMessageQueue::PostMessage(const std::string& message)
{
    int pthreadRet = pthread_mutex_lock(&qMutex);
    ASSERT(0 == pthreadRet);
    UNUSED(pthreadRet);
    q.push_back(message);
    pthreadRet = pthread_mutex_unlock(&qMutex);
    ASSERT(0 == pthreadRet);
}

const std::string ThreadSafeMessageQueue::GetNextMessage(void)
{
    std::string nextMessage = "";
    int pthreadRet = pthread_mutex_lock(&qMutex);
    ASSERT(0 == pthreadRet);
    UNUSED(pthreadRet);
    
    if(!q.empty())
    {
        nextMessage = q.front();
        q.pop_front();
    }
    
    pthreadRet = pthread_mutex_unlock(&qMutex);
    ASSERT(0 == pthreadRet);
    
    return nextMessage;
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
    //ASSERT(NULL != m_pClient);
    ASSERT(NULL != m_pQueue);
    ASSERT(NULL != m_pThread);
    
    const std::string nextMessage = m_pQueue->GetNextMessage();
    if(nextMessage == "signin")
    {
        std::cout << GetPeerName() 
        << "> Trying server: "
        << GetDefaultServerName() 
        << ":" << kDefaultServerPort 
        << std::endl;

        bool bStatus = m_pClient->Connect(
                            GetDefaultServerName(),
                            kDefaultServerPort,
                            GetPeerName()
                       );
        ASSERT(true == bStatus);
        UNUSED(bStatus);
    }
    else if(nextMessage == "signout")
    {
        std::cout << "Signing out..." << std::endl;
        
        bool bStatus = m_pClient->SignOut();
        ASSERT(true == bStatus);
        UNUSED(bStatus);
        m_pClient = NULL;
    }
    else if(nextMessage.find("sendhito") != std::string::npos)
    {
        int toPeerId;
        char dummy[10];
        sscanf(nextMessage.c_str(), "%d-%s",&toPeerId,dummy);
        
        for(Peers::const_iterator it = m_pClient->peers().begin();
            it!=m_pClient->peers().end();
            it++)
        {
            if(it->first == toPeerId)
            {
                m_pClient->SendToPeer(it->first, "hi there");
                break;
            }
        }
    }
    else if(nextMessage.find("connect") != std::string::npos)
    {
        int toPeerId;
        char dummy[10];
        sscanf(nextMessage.c_str(), "%d-%s",&toPeerId,dummy);
        
        for(Peers::const_iterator it = m_pClient->peers().begin();
            it!=m_pClient->peers().end();
            it++)
        {
            if(it->first == toPeerId)
            {
                m_pClient->GetPeerConnectionObserver()->ConnectToPeer(toPeerId);
                break;
            }
        }

    }
    else if(nextMessage == "hangup")
    {
        m_pClient->GetPeerConnectionObserver()->DisconnectFromCurrentPeer();
    }

    if(!m_pClient)
    {
        m_pThread->Quit();
    }

    return talk_base::PhysicalSocketServer::Wait(1000, process_io);
}