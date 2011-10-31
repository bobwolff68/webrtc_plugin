//
//  TestPeerConnectionClient.h
//  TestPeerConnectionClient
//
//  Created by Manjesh Malavalli on 10/14/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#ifndef TestPeerConnectionClient_TestPeerConnectionClient_h
#define TestPeerConnectionClient_TestPeerConnectionClient_h

#include <map>
#include <string>
#include "talk/base/sigslot.h"
#include "talk/base/scoped_ptr.h"
#include "talk/base/physicalsocketserver.h"

typedef std::map<int, std::string> Peers;
typedef std::map<std::string, std::string> ParsedCommand;

class TestPeerConnectionObserver;

class TestPeerConnectionClient : public sigslot::has_slots<> {
public:
    enum State {
        NOT_CONNECTED,
        SIGNING_IN,
        CONNECTED,
        SIGNING_OUT_WAITING,
        SIGNING_OUT,
    };
    
    TestPeerConnectionClient();
    TestPeerConnectionClient(const std::string& peerName,
                             const std::string& serverLocation,
                             const int serverPort);
    ~TestPeerConnectionClient();
    
    int id() const;
    bool is_connected() const;
    const Peers& peers() const;
    TestPeerConnectionObserver* GetPeerConnectionObserver(void) const;
    
    void RegisterPeerConnectionObserver(TestPeerConnectionObserver* pObserver);
    bool Connect(const std::string& server, int port,
                 const std::string& client_name);
    bool SendToPeer(int peer_id, const std::string& message);
    bool SendHangUp(int peer_id);
    bool IsSendingMessage();    
    bool SignOut();

    bool ExecuteNextCommand(ParsedCommand& cmd);

    
protected:
    void Close();
    bool ConnectControlSocket();
    void OnConnect(talk_base::AsyncSocket* socket);
    void OnHangingGetConnect(talk_base::AsyncSocket* socket);
    void OnMessageFromPeer(int peer_id, const std::string& message);
    
    // Quick and dirty support for parsing HTTP header values.
    bool GetHeaderValue(const std::string& data, size_t eoh,
                        const char* header_pattern, size_t* value);
    
    bool GetHeaderValue(const std::string& data, size_t eoh,
                        const char* header_pattern, std::string* value);
    
    // Returns true if the whole response has been read.
    bool ReadIntoBuffer(talk_base::AsyncSocket* socket, std::string* data,
                        size_t* content_length);
    
    void OnRead(talk_base::AsyncSocket* socket);
    
    void OnHangingGetRead(talk_base::AsyncSocket* socket);
    
    // Parses a single line entry in the form "<name>,<id>,<connected>"
    bool ParseEntry(const std::string& entry, std::string* name, int* id,
                    bool* connected);
    
    int GetResponseStatus(const std::string& response);
    
    bool ParseServerResponse(const std::string& response, size_t content_length,
                             size_t* peer_id, size_t* eoh);
    
    void OnClose(talk_base::AsyncSocket* socket, int err);
    
    talk_base::SocketAddress server_address_;
    talk_base::scoped_ptr<talk_base::AsyncSocket> control_socket_;
    talk_base::scoped_ptr<talk_base::AsyncSocket> hanging_get_;
    std::string onconnect_data_;
    std::string control_data_;
    std::string notification_data_;
    TestPeerConnectionObserver* m_pObserver;
    Peers peers_;
    State state_;
    int my_id_;
    
protected:
    std::string m_PeerName;
    std::string m_ServerLocation;
    int m_ServerPort;    
};

#endif
