/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

//  File: WPLPeerConnectionObserver.h
//  Project: WebrtcPlugin

#ifndef WebrtcPlugin_WPLPeerConnectionObserver_h
#define WebrtcPlugin_WPLPeerConnectionObserver_h

#include "WPLThreadSafeMessageQueue.h"
#include "talk/app/webrtc/peerconnection.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/base/scoped_ptr.h"

#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_LINUX))
#include "X11/WPLVideoRenderer.h"
#endif

#define GOCAST_AUDIO_IN   ""
#define GOCAST_AUDIO_OUT  ""
#define GOCAST_AUDIO_OPTS (cricket::MediaEngineInterface::ECHO_CANCELLATION |\
                           cricket::MediaEngineInterface::NOISE_SUPPRESSION |\
                           cricket::MediaEngineInterface::AUTO_GAIN_CONTROL)

namespace GoCast
{
    /**
    	Manages a voice connection with a single remote peer.
        It uses an instance of the webrtc::PeerConnection class to
        manage the capture/encode/transmit and receive/decode/render
        pipeline for the peer connection under its responsibility. It
        also forwards any signaling messages generated by the 
        webrtc::PeerConnection instance to the peer connection client,
        which then sends it to the appropriate remote peer (via the
        signin server). It also forwards signaling messages received from
        the remote peer to the webrtc::PeerConnection instance.
        
     */
    class PeerConnectionObserver : public webrtc::PeerConnectionObserver
    {
    public:
        /**
        	Constructor.
        	@param pMsgQ Message queue to store commands to 
                   GoCast::PeerConnectionClient.
            @param pWorkerThread Pointer to the peer connection 
                                 factory's worker thread.
            @param pPeerConnectionFactory Pointer to the global
                                          peer connection factory
                                          instance.
        	@returns N/A.
         */
        PeerConnectionObserver(
            ThreadSafeMessageQueue* pMsgQ,
            talk_base::scoped_ptr<talk_base::Thread>* pWorkerThread,
            talk_base::scoped_ptr<webrtc::PeerConnectionFactory>* pPeerConnectionFactory,
            bool bAudioOnly = true
        );
        
        /**
        	Destructor.
        	@returns N/A.
         */
        virtual ~PeerConnectionObserver();
        
        /**
        	Checks whether the voice connection with the remote peer
            is active.
        	@returns bool: 'true' if connection active, 'false' if not.
         */
        bool IsConnectionActive(void) const;
        
        /**
        	Set the unique peer id of the peer at the remote end of
            the connection (to be done once per instantiation).
        	@param peerId Unique remote peer id.
         */
        void SetPeerId(int peerId) { m_PeerId = peerId; }
        
        /**
        	Set the unique peer name of the peer at the remote end of
            the connection (to be done once per instantiation).
        	@param peerName Uniqe remote peer name.
         */
        void SetPeerName(const std::string& peerName) { m_PeerName = peerName; }
        
        /**
        	Callback used by GoCast::PeerConnectionClient to forward signaling
            messages from the remote peer.
        	@param peerId Unqiue peer id of the source of the signaling message.
        	@param msg The signaling message to be forwarded to the observer.
         */
        virtual void OnMessageFromRemotePeer(int peerId, const std::string& msg);
        
        /**
        	Attempts to set up a voice connection with the desired remote peer.
        	@param peerId Unique id of the remote peer.
        	@param peerName Uniqe name of the remote peer.
        	@param bSetPreviewWindow 'true' if preview window required, 'false' if not.
        	@returns bool: 'true' if successful, 'false' if not.
         */
        virtual bool ConnectToPeer(int peerId, const std::string& peerName);
        
        /**
        	Initiates teardown of the voice connection with the remote peer.
        	@returns bool: 'true' if successful, 'false' if not.
         */
        virtual bool DisconnectFromCurrentPeer(void);
        
#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_LINUX))
        bool SetRemoteVideoRenderer(const std::string& streamId,
                                    ThreadSafeMessageQueue* pEvtQ);
#endif
        
    protected:
        /**
        	Initializes the webrtc::PeerConnection instance.
        	@returns bool: 'true' if successful, 'false' if not.
         */
        bool InitPeerConnection(void);
        
        /**
        	Deletes the webrtc::PeerConnection instance.
         */
        void DeletePeerConnection(void);
        
        /**
        	Callback used by webrtc::PeerConnection to report
            an unspecified error.
         */
        virtual void OnError(void);
        
        virtual void OnMessage(const std::string& msg) {}
        
        /**
        	Callback used by webrtc::PeerConnection in order
            to forward the generated signaling message to the remote
            peer.
        	@param msg Signaling message.
         */
        virtual void OnSignalingMessage(const std::string& msg);
        
        /**
        	Callback used by webrtc::PeerConnection after a remote 
            audio/video stream has been added.
        	@param streamId Unique descriptor of the a/v stream.
        	@param video 'true' if stream is video, 'false' if audio.
         */
        virtual void OnAddStream(const std::string& streamId, bool video);
        
        /**
        	Callback used by webrtc::PeerConnection one of the
            active remote audio/video streams have been removed.
        	@param streamId Unique descriptor of the a/v stream.
        	@param video 'tru' if stream is video, 'false' if not.
         */
        virtual void OnRemoveStream(const std::string& streamId, bool video);
        
    protected:
        /**
        	Message queue to store commands to GoCast::PeerConnectionClient.
         */
        ThreadSafeMessageQueue* m_pMsgQ;
        
        /**
        	Scoped reference to an instance of webrtc::PeerConnection. This
            is the wokhorse that actually manages the media pipeline.
         */
        talk_base::scoped_ptr<webrtc::PeerConnection> m_pPeerConnection;
        
        /**
            Scoped reference to the worker thread used by webrtc::PeerConnection
            to manage the media pipeline.
         */
        talk_base::scoped_ptr<talk_base::Thread>* m_pWorkerThread;

        /**
        	Factory class that generates instances of webrtc::PeerConnection.
         */
        talk_base::scoped_ptr<webrtc::PeerConnectionFactory>* m_pPeerConnectionFactory;
                
        /**
        	Unique id of the remote peer.
         */
        int m_PeerId;
        
        /**
        	Unique name of the remote peer.
         */
        std::string m_PeerName;
        
        bool m_bAudioOnly;
        
#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_LINUX))
    protected:
        VideoRenderer* m_pRemoteRenderer;
#endif

    };
}

#endif
