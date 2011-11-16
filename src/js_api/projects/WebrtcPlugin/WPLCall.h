//
//  File: WPLCall.h
//  Project: WebrtcPlugin
//
//  Created by Manjesh Malavalli on 11/3/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#ifndef WebrtcPlugin_WPLCall_h
#define WebrtcPlugin_WPLCall_h

#include <map>
#include <string>
#include "WPLThreadSafeMessageQueue.h"
#include "WPLPeerConnectionObserver.h"

namespace GoCast
{
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
        bool AddParticipant(int peerId, std::string& peerName, bool bRemoteCall);
        
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
        
        void ListParticipants(void);
        
    protected:
        /**
        	List of participants (peerid, peername).
         */
        Participants m_Participants;
        
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
    };
}

#endif
