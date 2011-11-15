//
//  TestSocketServer.h
//  TestPeerConnectionClient
//
//  Created by Manjesh Malavalli on 10/18/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#ifndef TestPeerConnectionClient_TestSocketServer_h
#define TestPeerConnectionClient_TestSocketServer_h

#include "WPLThreadSafeMessageQueue.h"
#include "talk/base/thread.h"
#include "talk/base/physicalsocketserver.h"
#include "WPLPeerConnectionClient.h"

namespace GoCast
{
    class SocketServer : public talk_base::PhysicalSocketServer
    {
    protected:
        talk_base::Thread* m_pThread;
        PeerConnectionClient* m_pClient;
        
    public:
        SocketServer();
        virtual ~SocketServer();
        void SetPeerConnectionClient(PeerConnectionClient* pClient);
        virtual bool Wait(int cms, bool process_io);
    };
}

#endif
