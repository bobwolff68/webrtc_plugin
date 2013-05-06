//
//  GCPWebrtcCenter.h
//  FireBreath
//
//  Created by Manjesh Malavalli on 6/25/12.
//  Copyright (c) 2012 XVDTH. All rights reserved.
//

#ifndef FireBreath_GCPWebrtcCenter_h
#define FireBreath_GCPWebrtcCenter_h

#include <deque>
#include <map>

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include "talk/app/webrtc/videosourceinterface.h"
#include "talk/app/webrtc/peerconnection.h"
#include "talk/base/physicalsocketserver.h"
#include "talk/base/scoped_ptr.h"
#include "talk/base/thread.h"
#include "JSAPIAuto.h"

namespace GoCast
{
    class GCPVideoProcessor;
    std::string GetSigStateString(webrtc::PeerConnectionInterface::SignalingState state);
    std::string GetIceConnStateString(webrtc::PeerConnectionInterface::IceConnectionState state);
    std::string GetIceGathStateString(webrtc::PeerConnectionInterface::IceGatheringState state);
    
    class MessageHandler
    {
    public:
        virtual void OnMessage(talk_base::Message* pMsg) = 0;
    };
    
    class MessageQueue : public talk_base::PhysicalSocketServer
    {
    public:
        MessageQueue(MessageHandler* m_pHandler);
        virtual ~MessageQueue();
        
        //Public methods
        void Start();
        void Send(int msgType, talk_base::MessageData* pArgs, bool bWait = false);
        
        //Override PhysicalSocketServer's Wait()
        virtual bool Wait(int cms, bool bProcessIO);
        
        //Worker function
        void WorkerFunction();
    
    private:
        //Private methods
        talk_base::Message* Recv();
        void ProcessMessage();
        
    private:
        MessageHandler* m_pHandler;
        boost::mutex m_mutex;
        boost::thread m_thread;
        boost::condition m_done;
        std::deque<talk_base::Message*> m_messages;
    };
    
    class RtcCenter : public MessageHandler
    {
    public:
        static RtcCenter* Instance(bool bDelete = false);
        
        // Query media devices
        void QueryVideoDevices(FB::VariantMap& devices);
        void QueryAudioDevices(FB::VariantList& devices, bool bInput = true);
        
        //Thread-safe methods        
        void GetUserMedia(FB::JSObjectPtr mediaHints,
                          FB::JSObjectPtr succCb,
                          FB::JSObjectPtr failCb,
                          bool bSyncCall = true);
        bool NewPeerConnection(const std::string& pluginId,
                               FB::VariantList iceServers,
                               webrtc::PeerConnectionObserver* pObserver,
                               bool bSyncCall = true);
        bool AddStream(const std::string& pluginId,
                       const std::string& label,
                       bool bSyncCall = true);
        bool RemoveStream(const std::string& pluginId,
                          const std::string& label,
                          bool bSyncCall = true);
        void CreateOffer(const std::string& pluginId,
                         webrtc::CreateSessionDescriptionObserver* pObserver,
                         FB::JSObjectPtr constraints,
                         bool bSyncCall = true);
        void CreateAnswer(const std::string& pluginId,
                          webrtc::CreateSessionDescriptionObserver* pObserver,
                          FB::JSObjectPtr constraints,
                          bool bSyncCall = true);
        void SetLocalDescription(const std::string& pluginId,
                                 webrtc::SetSessionDescriptionObserver* pObserver,
                                 const std::string& action,
                                 const std::string& sdp,
                                 bool bSyncCall = true);
        void SetRemoteDescription(const std::string& pluginId,
                                  webrtc::SetSessionDescriptionObserver* pObserver,
                                  const std::string& action,
                                  const std::string& sdp,
                                  bool bSyncCall = true);
        bool AddIceCandidate(const std::string& pluginId,
                             const std::string& sdpMid,
                             const int sdpMlineIndex,
                             const std::string& candidateSdp,
                             bool bSyncCall = true);
        bool DeletePeerConnection(const std::string& pluginId,
                                  bool bSyncCall = true);
        
    public:
        std::string SignalingState(const std::string& pluginId);
        std::string IceConnectionState(const std::string& pluginId);
        std::string IceGatheringState(const std::string& pluginId);
        
        bool Inited() const;
        bool GetLocalVideoTrackEnabled() const;        
        bool GetLocalAudioTrackEnabled() const;
        bool GetSpkVol(int* pLevel) const;
        bool GetSpkMute(bool* pbEnabled) const;
        bool GetMicVol(int* pLevel) const;
        void SetLocalVideoTrackEnabled(bool bEnable);
        void SetLocalAudioTrackEnabled(bool bEnable);
        bool SetSpkVol(int level);
        bool SetMicVol(int level);
        void SetLocalVideoTrackRenderer(webrtc::VideoRendererInterface* pRenderer);
        void SetRemoteVideoTrackRenderer(const std::string& pluginId,
                                         webrtc::VideoRendererInterface* pRenderer);
        void AddRemoteStream(const std::string& pluginId,
                             const talk_base::scoped_refptr<webrtc::MediaStreamInterface>& pStream);
        void RemoveRemoteStream(const std::string& pluginId);

    private:
        RtcCenter();
        virtual ~RtcCenter();
                
        //talk_base::MessageHandler implementation
        void OnMessage(talk_base::Message* pMsg);
        
        //Methods that correspond to thread-safe methods
        void GetUserMedia_w(FB::JSObjectPtr mediaHints,
                            FB::JSObjectPtr succCb,
                            FB::JSObjectPtr failCb);
        bool NewPeerConnection_w(const std::string& pluginId,
                                 FB::VariantList iceServers,
                                 webrtc::PeerConnectionObserver* pObserver);
        bool AddStream_w(const std::string& pluginId,
                         const std::string& label);
        bool RemoveStream_w(const std::string& pluginId,
                            const std::string& label);
        void CreateOffer_w(const std::string& pluginId,
                           webrtc::CreateSessionDescriptionObserver* pObserver,
                           FB::JSObjectPtr constraints);
        void CreateAnswer_w(const std::string& pluginId,
                            webrtc::CreateSessionDescriptionObserver* pObserver,
                            FB::JSObjectPtr constraints);
        void SetLocalDescription_w(const std::string& pluginId,
                                   webrtc::SetSessionDescriptionObserver* pObserver,
                                   const std::string& action,
                                   const std::string& sdp);
        void SetRemoteDescription_w(const std::string& pluginId,
                                    webrtc::SetSessionDescriptionObserver* pObserver,
                                    const std::string& action,
                                    const std::string& sdp);
        bool AddIceCandidate_w(const std::string& pluginId,
                               const std::string& sdpMid,
                               const int sdpMlineIndex,
                               const std::string& candidateSdp);
        bool DeletePeerConnection_w(const std::string& pluginId);
        
    private:
        MessageQueue m_msgq;
        GCPVideoProcessor* m_pVideoProc;
        std::map< std::string,
                  talk_base::scoped_refptr<webrtc::PeerConnectionInterface> > m_pPeerConns;
        std::map< std::string,
                  talk_base::scoped_refptr<webrtc::MediaStreamInterface> > m_remoteStreams;
        talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pConnFactory;
        talk_base::scoped_refptr<webrtc::LocalMediaStreamInterface> m_pLocalStream;
    };
}

#endif
