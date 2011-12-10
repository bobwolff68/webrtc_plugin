/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

//  File: WPLCall.cpp
//  Project: WebrtcPlugin

#include <assert.h>
#include <iostream>
#include "WPLCall.h"
#include "talk/session/phone/mediaengine.h"
#include "talk/session/phone/webrtcvoiceengine.h"
#include "talk/p2p/client/basicportallocator.h"

#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_LINUX))
#include "talk/session/phone/webrtcvideoengine.h"
#endif


namespace GoCast
{
    cricket::MediaEngineInterface* MediaEngineFactory::Create()
    {

#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_LINUX))
        return new cricket::CompositeMediaEngine
        <cricket::WebRtcVoiceEngine,
        cricket::WebRtcVideoEngine>();
#else
        return new cricket::CompositeMediaEngine
        <cricket::WebRtcVoiceEngine,
        cricket::NullVideoEngine>();
#endif

    }
    
    cricket::DeviceManagerInterface* DeviceManagerFactory::Create()
    {
        return new cricket::DeviceManager();
    }
    
    Call::Call(ThreadSafeMessageQueue* pMsgQ,
               ThreadSafeMessageQueue* pEvtQ):
    m_pMsgQ(pMsgQ),
    m_pEvtQ(pEvtQ)
    
#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_LINUX))    
    ,m_pLocalRenderer(NULL)
#endif
    
    {
    
    }

    Call::~Call()
    {

    }

    bool Call::AddParticipant(int peerId, std::string& peerName, bool bRemoteCall)
    {
        if(m_Observers.end() != m_Observers.find(peerId))
        {
            return false;
        }
        
#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_LINUX))    
        if(true == m_Participants.empty())
        {
            std::string title = "me";
            m_pLocalRenderer = VideoRenderer::Create(
                                    title,
                                    GOCAST_DEFAULT_RENDER_WIDTH,
                                    GOCAST_DEFAULT_RENDER_HEIGHT
                               );
            
            if(false == m_pLocalRenderer->Init())
            {
                VideoRenderer::Destroy(m_pLocalRenderer);
                return false;
            }
            
            m_pMediaEngine->SetVideoCapture(true);
            m_pMediaEngine->SetLocalRenderer(m_pLocalRenderer);
        }
#endif

        m_Participants[peerId] = peerName;
        m_Observers[peerId] = new PeerConnectionObserver(
                                        m_pMsgQ,
                                        &m_pWorkerThread,
                                        &m_pPeerConnectionFactory
                                  );
        
        if(false == bRemoteCall)
        {
            if(false == m_Observers[peerId]->ConnectToPeer(peerId,peerName))
            {
                RemoveParticipant(peerId, true);
                return false;
            }
        }
        else
        {
            m_Observers[peerId]->SetPeerId(peerId);
            m_Observers[peerId]->SetPeerName(peerName);
            
            if(NULL != m_pEvtQ)
            {
                ThreadSafeMessageQueue::ParsedMessage event;
                event["type"] = "RemotePeerCall";
                event["message"] = peerName;
                m_pEvtQ->PostMessage(event);
            }
        }

        ListParticipants();
        
        return true;
    }

    bool Call::RemoveParticipant(int peerId, bool bRemoteHangup)
    {
        bool bStatus = false;
        
        if(m_Observers.end() == m_Observers.find(peerId))
        {
            return false;
        }
        
        if(true == bRemoteHangup)
        {
            bStatus = true;

            if(NULL != m_pEvtQ)
            {
                ThreadSafeMessageQueue::ParsedMessage event;
                event["type"] = "RemotePeerHangup";
                event["message"] = m_Participants[peerId];
                m_pEvtQ->PostMessage(event);
            }
        }
        else
        {
            bStatus = m_Observers[peerId]->DisconnectFromCurrentPeer();
        }
        
        if(true == bStatus)
        {
            delete m_Observers[peerId];
            m_Observers.erase(peerId);
            m_Participants.erase(peerId);
            ListParticipants();
            
#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_LINUX))
            if(true == m_Participants.empty())
            {
                m_pMediaEngine->SetLocalRenderer(NULL);
                m_pLocalRenderer->Deinit();
                VideoRenderer::Destroy(m_pLocalRenderer);
                m_pLocalRenderer = NULL;
                m_pMediaEngine->SetVideoCapture(false);
            }
#endif
            
        }
        else
        {
            std::cerr << __FUNCTION__ << ": DisconnectFromCurrentPeer() error..." << std::endl;
        }
        
        return bStatus;
    }

    bool Call::IsActive(void) const
    {
        return (false == m_Participants.empty());
    }

    bool Call::HasParticipant(int peerId) const
    {
        return (m_Participants.end() != m_Participants.find(peerId));
    }

    void Call::ListParticipants(void)
    {
        std::cout << std::endl << "===== CALL PARTICIPANTS =====" << std::endl;
        
        for(Participants::iterator it = m_Participants.begin();
            it != m_Participants.end();
            it++)
        {
            std::cout << "[ " << it->second << " ]" << std::endl;
        }
    }

    void Call::OnMessageFromPeer(int peerId, const std::string &msg)
    {
        m_Observers[peerId]->OnMessageFromRemotePeer(peerId, msg);
    }
    
    bool Call::InitPeerConnectionFactory()
    {
        ASSERT(NULL == m_pPeerConnectionFactory.get());
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
        
        //Create media engine and device manager
        m_pMediaEngine = MediaEngineFactory::Create();
        m_pDeviceManager = DeviceManagerFactory::Create();
        
        //Create peer connection factory
        m_pPeerConnectionFactory.reset(
            new webrtc::PeerConnectionFactory(
                        pPortAllocator,
                        m_pMediaEngine,
                        m_pDeviceManager,
                        m_pWorkerThread.get()
            )
        );
        
        if(false == m_pPeerConnectionFactory->Initialize())
        {
            std::cerr << __FUNCTION__ 
            << ": Unable to create peer connection factory"
            << std::endl;
            return false;
        }
        
        return true;
    }
    
    void Call::DeInitPeerConnectionFactory()
    {
        m_pWorkerThread.reset();
        m_pPeerConnectionFactory.reset();
    }
    
#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_LINUX))
    bool Call::SetRemoteVideoRenderer(const int peerId, const std::string& streamId)
    {
        if(!HasParticipant(peerId))
        {
            return false;
        }
        
        return m_Observers[peerId]->SetRemoteVideoRenderer(streamId);
    }
#endif

}
