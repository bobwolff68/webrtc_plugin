/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

//  File: WPLCall.h
//  Project: WebrtcPlugin

#ifndef WebrtcPlugin_WPLCall_h
#define WebrtcPlugin_WPLCall_h

#include <map>
#include <string>
#include "WPLThreadSafeMessageQueue.h"
#include "WPLPeerConnectionObserver.h"

#if(defined(GOCAST_ENABLE_VIDEO))

#if(defined(GOCAST_LINUX))
#include "X11/WPLVideoRenderer.h"
#endif

#if(defined(GOCAST_MAC))
#include "Mac/WPLVideoRenderer.h"
#endif

#endif

namespace GoCast
{
    class MediaEngineFactory
    {
    public:
        static cricket::MediaEngineInterface* Create();
    };
    
    class DeviceManagerFactory
    {
    public:
        static cricket::DeviceManagerInterface* Create();
    };
    
    /**
    	Handles list of call participants and manages
        corresponding peer connection observers. This class
        enables the setup of multiple point-to-point audio
        connections.
     */
    class Call
    {
    public:
        typedef std::map<int,std::string> Participants;
        typedef std::map<int,PeerConnectionObserver*> Observers;
        
    public:
        /**
        	Constructor.
        	@param pMsgQ Queue to store commands for peer connection client.
        	@param pEvtQ Queue to store events to be fired into JavaScript.
        	@returns N/A.
         */
        Call(ThreadSafeMessageQueue* pMsgQ, ThreadSafeMessageQueue* pEvtQ);
        
        /**
        	Destructor.
        	@returns N/A.
         */
        ~Call();
        
        /**
        	Add a peer to the call. Note that this merely starts
            the process of creating a voice connection with the specified
            peer. It DOES NOT create a conference call with other connected peers.
        	@param peerId Unique id of the peer to be added.
        	@param peerName Unique name of the peer to be added.
        	@param bRemoteCall 'true' if added peer requested a call, 'false' if not.
        	@returns 'true' if successful, 'false' if not.
         */
        bool AddParticipant(int peerId,
                            std::string& peerName, 
                            bool bRemoteCall,
                            bool bAudioOnly = true);
        
        /**
        	Remove a peer from the call.
        	@param peerId Unique id of the peer to be removed
        	@param bRemoteHangup 'true' if removed peer initiated the hangup, 'false' if not.
        	@returns 'true' if successful, 'false' if not.
         */
        bool RemoveParticipant(int peerId, bool bRemoteHangup);
        
        /**
        	Checks whether there are any active participants in the call (call is active).
        	@returns 'true' if call is active, 'false' if not.
         */
        bool IsActive(void) const;
        
        /**
        	Checks whether the specified peer is connected.
        	@param peerId Unique id of the peer.
        	@returns 'true' if connected, 'false' if not.
         */
        bool HasParticipant(int peerId) const;
        
        /**
        	Callback used by peer connection client to forward
            signaling messages.
        	@param peerId Unique id of the peer for which the signaling
                   message is intended.
        	@param msg Signaling message.
         */
        void OnMessageFromPeer(int peerId, const std::string& msg);
        
        /**
        	Lists the participants on the call.
         */
        void ListParticipants(void);
        
        /**
        	Initializes the peer connection factory.
        	@returns 'true' if successful, 'false' if not.
         */
        bool InitPeerConnectionFactory();
        
        /**
        	Deinitializes the peer connection factory.
         */
        void DeInitPeerConnectionFactory();
        
#if(defined(GOCAST_ENABLE_VIDEO) && !defined(GOCAST_WINDOWS))
        bool SetRemoteVideoRenderer(const int peerId, const std::string& streamId);
#endif
        
    protected:
        /**
        	List of participants (peerid, peername).
         */
        Participants m_Participants;
        
        Participants m_AVParticipants;
        
        /**
        	List of peer connection observers (peerid, GoCast::PeerConnectionObserver*).
         */
        Observers m_Observers;
        
        /**
        	Message queue to store commands for the peer connection client.
         */
        ThreadSafeMessageQueue* m_pMsgQ;
        
        /**
        	Message queue to store events to be fired into JavaScript.
            NOTE: Pass NULL in constructor if not using event queue.
         */
        ThreadSafeMessageQueue* m_pEvtQ;

        /**
        	Interface to the media engine which manages the audio/
            video channels of all peer connections.
         */
        cricket::MediaEngineInterface* m_pMediaEngine;
        
        /**
        	Interface to the device manager which manages
            the link between the audio/video channels of the 
            peer connections and the hardware input/output devices.
         */
        cricket::DeviceManagerInterface* m_pDeviceManager;
        
        /**
            Scoped reference to the worker thread used by webrtc::PeerConnection
            to manage the media pipeline.
         */
        talk_base::scoped_ptr<talk_base::Thread> m_pWorkerThread;
        
        /**
            Factory class that generates instances of webrtc::PeerConnection.
         */
        talk_base::scoped_ptr<webrtc::PeerConnectionFactory> m_pPeerConnectionFactory;
        
#if(defined(GOCAST_ENABLE_VIDEO) && !defined(GOCAST_WINDOWS))
        VideoRenderer* m_pLocalRenderer;
#endif

};
}

#endif
