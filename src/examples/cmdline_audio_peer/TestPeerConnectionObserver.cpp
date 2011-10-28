//
//  TestPeerConnectionObserver.cpp
//  TestPeerConnectionClient
//
//  Created by Manjesh Malavalli on 10/20/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#include <iostream>
#include <sstream>
#include "TestDefaults.h"
#include "TestPeerConnectionObserver.h"
#include "TestPeerConnectionClient.h"
#include "TestSocketServer.h"
#include "talk/base/common.h"
#include "talk/p2p/client/basicportallocator.h"

TestPeerConnectionObserver::TestPeerConnectionObserver(TestPeerConnectionClient* pClient,ThreadSafeMessageQueue* pMsgQ):
m_pClient(pClient),
m_pMsgQ(pMsgQ),
m_PeerId(-1),
m_bAudioStreamShared(false)
{
    m_pClient->RegisterPeerConnectionObserver(this);
}

TestPeerConnectionObserver::~TestPeerConnectionObserver()
{
    
}

bool TestPeerConnectionObserver::IsConnectionActive(void) const
{
    return (NULL != m_pPeerConnection.get());
}

bool TestPeerConnectionObserver::InitPeerConnection(void)
{
    ASSERT(NULL == m_pPeerConnectionFactory.get());
    ASSERT(NULL == m_pPeerConnection.get());
    ASSERT(NULL == m_pWorkerThread.get());
    
    //Reset worker thread
    m_pWorkerThread.reset(new talk_base::Thread());
    if(false == m_pWorkerThread->SetName("ObserverWT", this) ||
       false == m_pWorkerThread->Start())
    {
        std::cerr << __FUNCTION__
                  << ": Unable to start libjingle worker thread"
                  << std::endl;
        m_pWorkerThread.reset();
        return false;
    }
    
    //Create port allocator of peer connection factory
    cricket::PortAllocator* pPortAllocator = NULL;
    pPortAllocator = new cricket::BasicPortAllocator(
                        new talk_base::BasicNetworkManager(),
                        talk_base::SocketAddress("stun.l.google.com", 19302),
                        talk_base::SocketAddress(),
                        talk_base::SocketAddress(),
                        talk_base::SocketAddress()
                     );
    
    //Create peer connection factory
    m_pPeerConnectionFactory.reset(
        new webrtc::PeerConnectionFactory(
                pPortAllocator,
                m_pWorkerThread.get()
            )
    );
    
    if(false == m_pPeerConnectionFactory->Initialize())
    {
        std::cerr << __FUNCTION__ 
                  << ": Unable to create peer connection factory"
                  << std::endl;
        DeletePeerConnection();
        return false;
    }
    
    //Create peer connection
    m_pPeerConnection.reset(
        m_pPeerConnectionFactory->CreatePeerConnection(
            m_pWorkerThread.get()
        )
    );
    
    if(NULL == m_pPeerConnection.get())
    {
        std::cerr << __FUNCTION__ 
        << ": Unable to create peer connection"
        << std::endl;
        DeletePeerConnection();
        return false;
    }
    
    m_pPeerConnection->RegisterObserver(this);
    bool bStatus = m_pPeerConnection->SetAudioDevice("", "", 0);
    std::cout << "SetAudioDevice "
              << (bStatus ? "succeeded" : "failed")
              << std::endl;
    
    return true;
}

void TestPeerConnectionObserver::DeletePeerConnection(void)
{
    m_pPeerConnection.reset();
    m_pWorkerThread.reset();
    m_pPeerConnectionFactory.reset();
    m_PeerId = -1;
}

void TestPeerConnectionObserver::Close(void)
{
    m_pClient->SignOut();
    DeletePeerConnection();
}

void TestPeerConnectionObserver::OnError(void)
{
    std::cerr << __FUNCTION__ << ": Unknown error" << std::endl;
}

void TestPeerConnectionObserver::OnSignalingMessage(const std::string& msg)
{
    if(-1 == m_PeerId)
    {
        std::cerr << __FUNCTION__ 
                  << ": Not connected to any peer yet"
                  << std::endl;
        return;
    }
    
    //std::cout << "PeerConnection says: " << msg << std::endl;
    //std::cout << "Forwarding to peer: " << m_PeerId << std::endl;
    
    //Send message from peer connection to destination peer
    //m_pClient->SendToPeer(m_PeerId, msg);
    std::stringstream sstrm;
    sstrm << m_PeerId;
    std::string peerMsg = sstrm.str() + "/";
    peerMsg += msg;
    m_pMsgQ->PostMessage(peerMsg);
}

void TestPeerConnectionObserver::OnAddStream(const std::string &streamId, bool video)
{
    if(false == video)
    {
        std::cout << "Peerconnection added remote stream: " 
                  << streamId 
                  << std::endl;
        ShareLocalAudioStream();
    }
}

void TestPeerConnectionObserver::OnRemoveStream(const std::string &streamId, bool video)
{
    std::cout << "Peerconnection removed remote stream: " 
              << streamId
              << std::endl;
    //m_pClient->SendHangUp(m_PeerId);
    m_pMsgQ->PostMessage("hangup");
    DeletePeerConnection();
}

void TestPeerConnectionObserver::OnMessageFromRemotePeer(int peerId, const std::string& msg)
{
    ASSERT(m_PeerId==peerId || -1==m_PeerId);
    ASSERT(false == msg.empty());
    
    if(msg == "hi there")
    {
        return;
    }
    
    if(NULL == m_pPeerConnection.get())
    {
        ASSERT(-1 == m_PeerId);
        m_PeerId = peerId;
        
        std::cout << "Call request from peer: " << peerId << std::endl;
        std::cout << "Offer: " << msg << std::endl;

        if(false == InitPeerConnection())
        {
            return;
        }
    }
    else if(peerId != m_PeerId)
    {
        ASSERT(-1 != m_PeerId);
        std::cerr << __FUNCTION__ << ": Local peer busy..." << std::endl;
        return;
    }

    m_pPeerConnection->SignalingMessage(msg);
}

void TestPeerConnectionObserver::ConnectToPeer(int peerId)
{
    if(-1!=m_PeerId || NULL!=m_pPeerConnection.get())
    {
        std::cerr << __FUNCTION__ << ": Local peer busy..." << std::endl;
        return;
    }
    
    if(false == InitPeerConnection())
    {
        return;
    }
    
    m_PeerId = peerId;
    ShareLocalAudioStream();
}

void TestPeerConnectionObserver::ShareLocalAudioStream(void)
{
    //If peer has not shared its stream with remote peer yet, share it
    if(false == m_bAudioStreamShared)
    {
        m_pPeerConnection->AddStream(kAudioLabel,false);
        m_pPeerConnection->Connect();
        m_bAudioStreamShared = true;
    }
}

void TestPeerConnectionObserver::DisconnectFromCurrentPeer(void)
{
    //m_pClient->SendHangUp(m_PeerId);
    m_pMsgQ->PostMessage("hangup");
    DeletePeerConnection();
}
