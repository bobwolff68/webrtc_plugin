//
//  Call.h
//  cmdline_audio_peer
//
//  Created by Manjesh Malavalli on 11/3/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#ifndef cmdline_audio_peer_Call_h
#define cmdline_audio_peer_Call_h

#include <map>
#include <string>
#include "WPLThreadSafeMessageQueue.h"
#include "WPLPeerConnectionObserver.h"

namespace projectname
{
    class Call
    {
    public:
        typedef std::map<int,std::string> Participants;
        typedef std::map<int,PeerConnectionObserver*> Observers;
        
    public:
        Call(ThreadSafeMessageQueue* pMsgQ,
             ThreadSafeMessageQueue* pEvtQ);
        ~Call();
        bool AddParticipant(int peerId, std::string& peerName, bool bRemoteCall);
        bool RemoveParticipant(int peerId, bool bRemoteHangup);
        bool IsActive(void) const;
        bool HasParticipant(int peerId) const;
        void OnMessageFromPeer(int peerId, const std::string& msg);
        void ListParticipants(void);
        
    protected:
        Participants m_Participants;
        Observers m_Observers;
        ThreadSafeMessageQueue* m_pMsgQ;
        ThreadSafeMessageQueue* m_pEvtQ;
    };
}

#endif
