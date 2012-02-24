/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

//  File WPLSocketServer.cpp
//  Project: WebrtcPlugin

#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_LINUX))
#include <gtk/gtk.h>
#endif

#include <iostream>
#include <stdio.h>
#include "WPLSocketServer.h"

namespace GoCast
{
    SocketServer::SocketServer():
    m_pThread(talk_base::Thread::Current()),
    m_pClient(NULL)
    {

    }

    SocketServer::~SocketServer()
    {
        
    }

    void SocketServer::SetPeerConnectionClient(PeerConnectionClient *pClient)
    {
        ASSERT(NULL == m_pClient);
        m_pClient = pClient;
    }

    bool SocketServer::Wait(int cms, bool process_io)
    {
        ASSERT(NULL != m_pThread);
        
#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_LINUX))
        while(gtk_events_pending())
        {
            gtk_main_iteration();
        }
#endif

        bool bQuitCommand = false;
        bool bStatus = m_pClient->ExecuteNextCommand(bQuitCommand);    
        
        if(true == bQuitCommand && true == bStatus)
        {
            std::cout << "Quitting..." << std::endl;
            m_pThread->Quit();
        }
        
        return talk_base::PhysicalSocketServer::Wait(20, process_io);
    }
}

