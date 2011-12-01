/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

//  File: PeerConnectionObserver.cpp
//  Project: WebrtcPlugin

#include <iostream>
#include <sstream>
#include "WPLPeerConnectionObserver.h"
#include "WPLPeerConnectionClient.h"
#include "WPLSocketServer.h"
#include "rtc_common.h"
#include "talk/base/common.h"
#include "talk/session/phone/mediaengine.h"

namespace GoCast
{
    PeerConnectionObserver::PeerConnectionObserver(
        ThreadSafeMessageQueue* pMsgQ,
        talk_base::scoped_ptr<talk_base::Thread>* pWorkerThread,
        talk_base::scoped_ptr<webrtc::PeerConnectionFactory>* pPeerConnectionFactory
    ):  m_pMsgQ(pMsgQ),
        m_pWorkerThread(pWorkerThread),
        m_pPeerConnectionFactory(pPeerConnectionFactory),
        m_PeerId(-1),
        m_PeerName("")
    {

    }

    PeerConnectionObserver::~PeerConnectionObserver()
    {
        
    }

    bool PeerConnectionObserver::IsConnectionActive(void) const
    {
        return (NULL != m_pPeerConnection.get());
    }

    bool PeerConnectionObserver::InitPeerConnection(void)
    {
        ASSERT(NULL == m_pPeerConnection.get());
                        
        //Create peer connection
        m_pPeerConnection.reset(
            (*m_pPeerConnectionFactory)->CreatePeerConnection(
                m_pWorkerThread->get()
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
        bool bStatus = m_pPeerConnection->SetAudioDevice(
                            GOCAST_AUDIO_IN,
                            GOCAST_AUDIO_OUT,
                            GOCAST_AUDIO_OPTS
                       );
        
        std::cout << "SetAudioDevice "
                  << (bStatus ? "succeeded" : "failed")
                  << std::endl;
        
        return bStatus;
    }

    void PeerConnectionObserver::DeletePeerConnection(void)
    {
        m_pPeerConnection.reset();
        m_PeerId = -1;
        m_PeerName = "";
    }

    void PeerConnectionObserver::OnError(void)
    {
        std::cerr << __FUNCTION__ << ": Unknown error" << std::endl;
    }

    void PeerConnectionObserver::OnSignalingMessage(const std::string& msg)
    {
        ParsedMessage sendCmd;
        std::stringstream sstrm;
        sstrm << m_PeerId;
        sendCmd["command"] = "sendtopeer";
        sendCmd["message"] = msg;
        sendCmd["peerid"] = sstrm.str();
        m_pMsgQ->PostMessage(sendCmd);
    }

    void PeerConnectionObserver::OnAddStream(const std::string &streamId, bool video)
    {
        if(false == video)
        {
            std::cout << m_PeerName << " added local stream: " 
                      << streamId 
                      << std::endl;
        }
    }

    void PeerConnectionObserver::OnRemoveStream(const std::string &streamId, bool video)
    {
        std::cout << m_PeerName << "removed local stream: " 
                  << streamId
                  << std::endl;
    }

    void PeerConnectionObserver::OnMessageFromRemotePeer(int peerId, const std::string& msg)
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
                    ParsedMessage cmd;
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

    void PeerConnectionObserver::ConnectToPeer(int peerId, const std::string& peerName)
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
        m_pPeerConnection->AddStream("local_audio",false);
        m_pPeerConnection->Connect();
    }

    bool PeerConnectionObserver::DisconnectFromCurrentPeer(void)
    {
        std::cout << "Hanging up..." << std::endl;
        int remotePeerId = m_PeerId;
        
        if(true == m_pPeerConnection->Close())
        {
            DeletePeerConnection();
            ParsedMessage cmd;
            cmd["command"] = "sendtopeer";
            cmd["peerid"] = ToString(remotePeerId);
            cmd["message"] = "bye";
            m_pMsgQ->PostMessage(cmd);
            return true;
        }

        std::cerr << __FUNCTION__ << ": Peer connection close error..." << std::endl;
        return false;
    }
}