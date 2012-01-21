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
        talk_base::scoped_ptr<webrtc::PeerConnectionFactory>* pPeerConnectionFactory,
        bool bAudioOnly
    ):  m_pMsgQ(pMsgQ),
        m_pWorkerThread(pWorkerThread),
        m_pPeerConnectionFactory(pPeerConnectionFactory),
        m_PeerId(-1),
        m_PeerName(""),
        m_bAudioOnly(bAudioOnly)

#if(defined(GOCAST_ENABLE_VIDEO) && !defined(GOCAST_WINDOWS))
        ,m_pRemoteRenderer(NULL)
#endif

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
        
        if(false == bStatus)
        {
            std::cout << "SetAudioDevice "
                      << (bStatus ? "succeeded" : "failed")
                      << std::endl;
            return false;
        }
        
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
        std::cout << m_PeerName << " added local " 
                  << (video ? "video" : "voice") 
                  << " stream: " << streamId
                  << std::endl;
                  
#if(defined(GOCAST_ENABLE_VIDEO) && !defined(GOCAST_WINDOWS))
        if(true==video && NULL==m_pRemoteRenderer)
        {
            ParsedMessage cmd;
            cmd["command"] = "setremoterenderer";
            cmd["peerid"] = ToString(m_PeerId);
            cmd["streamid"] = streamId;
            m_pMsgQ->PostMessage(cmd);
        }
#endif
                  
    }

    void PeerConnectionObserver::OnRemoveStream(const std::string &streamId, bool video)
    {
        std::cout << m_PeerName << " removed local " 
                  << (video ? "video" : "voice") 
                  << " stream: " << streamId
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
                
#if(defined(GOCAST_ENABLE_VIDEO) && !defined(GOCAST_WINDOWS))
                m_pPeerConnection->SetVideoRenderer("video", NULL);
                if(NULL != m_pRemoteRenderer)
                {
                    m_pRemoteRenderer->Deinit();
                    VideoRenderer::Destroy(m_pRemoteRenderer);
                    m_pRemoteRenderer = NULL;
                }
#endif
                
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

    bool PeerConnectionObserver::ConnectToPeer(int peerId, const std::string& peerName)
    {
        if(IsConnectionActive())
        {
            std::cerr << __FUNCTION__ << ": Local peer busy..." << std::endl;
            return false;
        }
        
        if(false == InitPeerConnection())
        {
            std::cerr << __FUNCTION__ << ": InitPeerConnection() failed..." << std::endl;
            return false;
        }
        
        m_PeerId = peerId;
        m_PeerName = peerName;
        m_pPeerConnection->AddStream("voice",false);
        
#if(defined(GOCAST_ENABLE_VIDEO) && !defined(GOCAST_WINDOWS))
        if(false == m_bAudioOnly)
        {
            m_pPeerConnection->AddStream("video",true);
        }
#endif
        
        m_pPeerConnection->Connect();
        return true;
    }

    bool PeerConnectionObserver::DisconnectFromCurrentPeer(void)
    {
        std::cout << "Hanging up..." << std::endl;
        int remotePeerId = m_PeerId;
        
#if(defined(GOCAST_ENABLE_VIDEO) && !defined(GOCAST_WINDOWS))
        m_pPeerConnection->SetVideoRenderer("video", NULL);
        if(NULL != m_pRemoteRenderer)
        {
            m_pRemoteRenderer->Deinit();
            VideoRenderer::Destroy(m_pRemoteRenderer);
            m_pRemoteRenderer = NULL;
        }
#endif

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
    
#if(defined(GOCAST_ENABLE_VIDEO) && !defined(GOCAST_WINDOWS))
        bool PeerConnectionObserver::SetRemoteVideoRenderer(const std::string& streamId,
                                                            ThreadSafeMessageQueue* pEvtQ)
        {
            m_pRemoteRenderer = VideoRenderer::Create(
                                    m_PeerName,
                                    GOCAST_DEFAULT_RENDER_WIDTH,
                                    GOCAST_DEFAULT_RENDER_HEIGHT,
                                    pEvtQ
                                );
            bool bStatus = m_pRemoteRenderer->Init();
            if(false == bStatus)
            {
                std::cerr << __FUNCTION__ << ": remote renderer init failed..." << std::endl;
                return false;
            }
            
            return m_pPeerConnection->SetVideoRenderer(streamId, m_pRemoteRenderer);
        }
#endif

}

