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
#include "rtc_common.h"
#include "talk/base/common.h"
#include "talk/p2p/client/basicportallocator.h"

TestPeerConnectionObserver::TestPeerConnectionObserver(ThreadSafeMessageQueue* pMsgQ):
m_pMsgQ(pMsgQ),
m_PeerId(-1),
m_PeerName(""),
m_bAudioStreamShared(false)
{

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
    m_PeerName = "";
}

void TestPeerConnectionObserver::OnError(void)
{
    std::cerr << __FUNCTION__ << ": Unknown error" << std::endl;
}

void TestPeerConnectionObserver::OnSignalingMessage(const std::string& msg)
{
    ParsedCommand sendCmd;
    std::stringstream sstrm;
    sstrm << m_PeerId;
    sendCmd["command"] = "sendtopeer";
    sendCmd["message"] = msg;
    sendCmd["peerid"] = sstrm.str();
    m_pMsgQ->PostMessage(sendCmd);
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

    m_bAudioStreamShared = false;
}

void TestPeerConnectionObserver::OnMessageFromRemotePeer(int peerId, const std::string& msg)
{
    if(msg == "bye")
    {
        if(IsConnectionActive())
        {
            std::cout << m_PeerName << " hung up..." << std::endl;
            int peerId = m_PeerId;
            if(true == m_pPeerConnection->Close())
            {
                DeletePeerConnection();
                ParsedCommand cmd;
                cmd["command"] = "deleteobserver";
                cmd["peerid"] = ToString(peerId);
                m_pMsgQ->PostMessage(cmd);
            }
            else
            {
                std::cerr << __FUNCTION__ << ": Connection teardown failed..." << std::endl;
            }
        }
        else
        {
            std::cerr << __FUNCTION__ << ": Peerconnection already deleted..." << std::endl;
        }
        
        return;
    }
    
    if(false == IsConnectionActive())
    {
        std::cout << m_PeerName << " on the line..." << std::endl;
        
        if(false == InitPeerConnection())
        {
            std::cerr << __FUNCTION__ << ": Failed to init peer connection..." << std::endl;
            return;
        }
    }

    m_pPeerConnection->SignalingMessage(msg);
}

void TestPeerConnectionObserver::ConnectToPeer(int peerId, const std::string& peerName)
{
    if(IsConnectionActive())
    {
        std::cerr << __FUNCTION__ << ": Local peer busy..." << std::endl;
        return;
    }
    
    if(false == InitPeerConnection())
    {
        return;
    }
    
    m_PeerId = peerId;
    m_PeerName = peerName;
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

bool TestPeerConnectionObserver::DisconnectFromCurrentPeer(void)
{
    std::cout << "Hanging up..." << std::endl;
    int remotePeerId = m_PeerId;
    
    if(true == m_pPeerConnection->Close())
    {
        DeletePeerConnection();
        ParsedCommand cmd;
        cmd["command"] = "sendtopeer";
        cmd["peerid"] = ToString(remotePeerId);
        cmd["message"] = "bye";
        m_pMsgQ->PostMessage(cmd);
        return true;
    }

    std::cerr << __FUNCTION__ << ": Peer connection close error..." << std::endl;
    return false;
}