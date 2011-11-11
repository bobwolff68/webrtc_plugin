//
//  Call.cpp
//  cmdline_audio_peer
//
//  Created by Manjesh Malavalli on 11/3/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#include <assert.h>
#include <iostream>
#include "WPLCall.h"

namespace projectname
{
    Call::Call(ThreadSafeMessageQueue* pMsgQ):
    m_pMsgQ(pMsgQ)
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
        
        m_Participants[peerId] = peerName;
        m_Observers[peerId] = new PeerConnectionObserver(m_pMsgQ);
        
        if(false == bRemoteCall)
        {
            m_Observers[peerId]->ConnectToPeer(peerId,peerName);
        }
        else
        {
            m_Observers[peerId]->SetPeerId(peerId);
            m_Observers[peerId]->SetPeerName(peerName);
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
}