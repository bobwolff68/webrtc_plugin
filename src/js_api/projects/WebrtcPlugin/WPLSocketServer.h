/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

//  File: WPLSocketServer.h
//  Project: WebrtcPlugin

#ifndef WebrtcPlugin_WPLSocketServer_h
#define WebrtcPlugin_WPLSocketServer_h

#include "WPLThreadSafeMessageQueue.h"
#include "talk/base/thread.h"
#include "talk/base/physicalsocketserver.h"
#include "WPLPeerConnectionClient.h"

namespace GoCast
{
    /**
    	Threaded socket factory class for communication with 
        the signin server. It is a customized talk_base::PhysicalSocketServer
        implementation specific to this functionality.
        
     */
    class SocketServer : public talk_base::PhysicalSocketServer
    {
    protected:
        
        /**
        	Pointer to the thread associated with this instace
            of GoCast::SocketServer.
         */
        talk_base::Thread* m_pThread;
        
        /**
        	Pointer to the peer connection client whose communication
            with the signin server is being handled by the socket server.
         */
        PeerConnectionClient* m_pClient;
        
    public:
        /**
        	Constructor.
        	@returns N/A.
         */
        SocketServer();
        
        /**
        	Destructor.
        	@returns N/A.
         */
        virtual ~SocketServer();
        
        /**
        	Assign the peer connection client whose communication with
            the signin server has to be handled (to be done once).
        	@param pClient Pointer to the peer connection client.
         */
        void SetPeerConnectionClient(PeerConnectionClient* pClient);
        
        /**
        	Customized wait() which overloads the 
            talk_base::PhysicalSocketServer::Wait() function. It basically
            executes one command of the peer connection client before
            calling the base class's wait().
        	@param cms Wait time in milliseconds.
        	@param process_io 'true' if I/O needs to be processed, 'false' if not.
        	@returns bool: 'true' if successful, 'false' if not.
         */
        virtual bool Wait(int cms, bool process_io);
    };
}

#endif

