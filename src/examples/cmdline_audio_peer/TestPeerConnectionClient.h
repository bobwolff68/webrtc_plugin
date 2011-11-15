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
#include "ThreadSafeMessageQueue.h"
#include "Call.h"
#include "talk/base/sigslot.h"
#include "talk/base/scoped_ptr.h"
#include "talk/base/physicalsocketserver.h"

typedef std::map<std::string, std::string> ParsedCommand;
typedef std::map<int, /**< id for a peer mapped to... */
std::string> Peers; /**< The peer name/description for each peer id. */

class TestPeerConnectionObserver;

/**
 @brief Using the QT-style signals and slots as its base, PeerConnectionClient keeps our
 connection state maintained. It also gives us access to initiate connection with
 a server, send messages to peers, and have a continuously updated list of peers.
 */
class TestPeerConnectionClient : public sigslot::has_slots<> {
public:
    /**
     The current state of the client connection. Note that these states are not just 'static'
     but also are transient in nature by their names (SIGNING_OUT_WAITING and SIGNING_OUT).
     */
    enum State {
        NOT_CONNECTED,
        SIGNING_IN,
        CONNECTED,
        SIGNING_OUT_WAITING,
        SIGNING_OUT,
    };
    
    TestPeerConnectionClient(ThreadSafeMessageQueue* pMsgQ,
                             const std::string& peerName,
                             const std::string& serverLocation,
                             const int serverPort);
    virtual ~TestPeerConnectionClient();
    
    /**
    	What is my peer id assigned from the server
    	@returns peer id for this client
     */
    int id() const;
    /**
    	Find out the connection state with the server. Yes or no.
    	@returns Whether or not we are connected to the server at this time.
     */
    bool is_connected() const;
    /**
    	The list of current peers - their id and description in a map<int, std::string>
    	@returns A reference to a map of int,string
     */
    const Peers& peers() const;
    
    /**
    	Make a new connection to a server.
    	@param server IP address of server. TODO Can this be a canonical name as well?
    	@param port Port number for connection
    	@param client_name The desired client name for this client. All peers will know this client by the requested name. TODO What to do about name collisions?
    	@returns true on success.
     */
    bool Connect(const std::string& server, int port,
                 const std::string& client_name);
    bool SendToPeer(int peer_id, const std::string& message);
    bool SignOut();

    bool ExecuteNextCommand(bool& bQuitCommand);

    
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
    //TestPeerConnectionObserver* m_pObserver;
    Call* m_pCall;
    ThreadSafeMessageQueue* m_pMsgQ;
    Peers peers_;
    State state_;
    int my_id_;
    
protected:
    std::string m_PeerName;
    std::string m_ServerLocation;
    int m_ServerPort;    
};

#endif
