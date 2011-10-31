//
//  TestPeerConnectionObserver.h
//  TestPeerConnectionClient
//
//  Created by Manjesh Malavalli on 10/20/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#ifndef TestPeerConnectionClient_TestPeerConnectionObserver_h
#define TestPeerConnectionClient_TestPeerConnectionObserver_h

#include "talk/app/webrtc/peerconnection.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/base/scoped_ptr.h"

class TestPeerConnectionClient;
class ThreadSafeMessageQueue; 

class TestPeerConnectionObserver : public webrtc::PeerConnectionObserver
{
public:
    //Constructor and destructor
    TestPeerConnectionObserver(ThreadSafeMessageQueue* pMsgQ);
    ~TestPeerConnectionObserver();
    
    //Get functions
    bool IsConnectionActive(void) const;
    
    //Class-specific functions
    virtual void OnMessageFromRemotePeer(int peerId, const std::string& msg);
    virtual void ConnectToPeer(int peerId);
    virtual void DisconnectFromCurrentPeer(void);

protected:
    //Member functions
    bool InitPeerConnection(void);
    void DeletePeerConnection(void);
    virtual void Close(void);
    
    //webrtc::PeerConnectionObserver implementation
    virtual void OnError(void);
    virtual void OnMessage(const std::string& msg) {}
    virtual void OnSignalingMessage(const std::string& msg);
    //virtual void OnStateChange(Readiness state) {}
    //virtual void OnAddStream(webrtc::MediaStream* stream);
    virtual void OnAddStream(const std::string& streamId, bool video);
    //virtual void OnRemoveStream(webrtc::MediaStream* stream);
    virtual void OnRemoveStream(const std::string& streamId, bool video);
    
    //Class-specific functions
    virtual void ShareLocalAudioStream(void);
    
protected:
    ThreadSafeMessageQueue* m_pMsgQ;
    talk_base::scoped_ptr<webrtc::PeerConnection> m_pPeerConnection;
    talk_base::scoped_ptr<webrtc::PeerConnectionFactory> m_pPeerConnectionFactory;
    talk_base::scoped_ptr<talk_base::Thread> m_pWorkerThread;
    int m_PeerId;
    bool m_bAudioStreamShared;
};

#endif
