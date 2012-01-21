/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

//  File: WPLPeerConnectionClient.h
//  Project: WebrtcPlugin

#ifndef WebrtcPlugin_WPLPeerConnectionClient_h
#define WebrtcPlugin_WPLPeerConnectionClient_h

#include <map>
#include <string>
#include <unistd.h>
#include "ThreadSingle.h"
#include "WPLThreadSafeMessageQueue.h"
#include "WPLCall.h"
#include "talk/base/sigslot.h"
#include "talk/base/scoped_ptr.h"
#include "talk/base/physicalsocketserver.h"

typedef std::map<int, std::string> Peers;
typedef std::map<std::string, std::string> ParsedMessage;

namespace GoCast
{   
    /**
    	Handles signaling between peers with the aid of a signin server.
        This class has been designed to interact with WebRTC's sample 
        'peerconnection_server' aka 'signin server'. The interaction 
        involves registering the client's peer name, followed by receiving
        a unique peer id. Right after signin, and from time to time,
        it also receives notifications from the server when someone
        comes online or goes offline. It also manages instances of
        the GoCast::PeerConnectionObserver class, which in turn
        manage the creation/teardown of the capture/encode/transmit
        and receive/decode/render pipelines. GoCast::PeerConnectionClient
        receives signaling messages from its observers and forwards them
        to the server, which in turn forwards them to the appropriate peers.
        It also forwards the signaling messages it receives from the server
        to the appropriate observers.
     */
    class PeerConnectionClient : public sigslot::has_slots<>
    {
    public:
        /**
        	Describes the different states of the GoCast::PeerConnectionClient
            instance.
         */
        enum State
        {
            NOT_CONNECTED,
            SIGNING_IN,
            CONNECTED,
            SIGNING_OUT_WAITING,
            SIGNING_OUT,
        };
        
        /**
        	Constructor.
        	@param pMsgQ Message queue to store commands to be executed.
        	@param pEvtQ Event queue to store generated events that are to be fired into JavaScript.
        	@param peerName Unique peer name.
        	@param serverLocation Public IP of the signin server.
        	@param serverPort Public port on which the signin server is listening.
        	@returns N/A
         */
        PeerConnectionClient(ThreadSafeMessageQueue* pMsgQ,
                             ThreadSafeMessageQueue* pEvtQ,
                             const std::string& peerName,
                             const std::string& serverLocation,
                             const int serverPort,
                             const bool bAudioOnly = true);
        
        /**
        	Destructor
        	@returns N/A.
         */
        virtual ~PeerConnectionClient();
        
        /**
        	Returns the unique peer id
        	@returns int: the unique peer id, or -1 if not signed in.
         */
        int id(void) const;
        
        /**
        	Checks if client is signed in to a signin server.
        	@returns bool: 'true' if connected, 'false' if not.
         */
        bool is_connected(void) const;
        
        /**
        	Returns the list of online peers
        	@returns std::map<int,std::string>: Map of unique peer id's and
                     their corresponding names.
         */
        const Peers& peers(void) const;
        
        /**
        	Signs in to a signin server. 
        	@param server Signin server's public ip address.
        	@param port Signin server's public port.
        	@param client_name Desired name of the client
        	@returns bool: 'true' if successful, 'false' if not.
         */
        bool Connect(const std::string& server, int port,
                     const std::string& client_name);
        
        /**
            Sends given message to the signin server instructing
            it to forward the message to theonline peer corresponding 
            to the given peer id.
            @param peer_id Unique id of the destination peer.
            @param message Message to be sent.
            @returns bool: 'true' if successful, 'false' if not.
         */
        bool SendToPeer(int peer_id,const std::string& message);

        /**
        	Signs out the peer client from the signin server.
        	@returns bool: 'true' if successful, 'false' if not.
         */
        bool SignOut();

        /**
        	Attempts to execute the next command from the command message queue.
        	@param bQuitCommand 'true' if function returns 'false' because 
                   the command is 'quit' or 'exit'.
        	@returns bool: 'true' if successful, 'false' if either unsuccessful or 
                     command is 'quit' or exit'.
         */
        bool ExecuteNextCommand(bool& bQuitCommand);
        
        bool InitPeerConnectionFactory();
        void DeInitPeerConnectionFactory();

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
        
    protected:
        talk_base::SocketAddress server_address_;
        talk_base::scoped_ptr<talk_base::AsyncSocket> control_socket_;
        talk_base::scoped_ptr<talk_base::AsyncSocket> hanging_get_;
        std::string onconnect_data_;
        std::string control_data_;
        std::string notification_data_;
        
    protected:
        /**
            Pointer to class which sets up, manages and tears down peer connection
            observers.
         */
        Call* m_pCall;

        /**
        	Message queue that stores commands to be executed by the
            GoCast::PeerConnecitonClient instance.
         */
        ThreadSafeMessageQueue* m_pMsgQ;
        
        /**
        	Event queue to store generated events that are to be fired into JavaScript.
            NOTE: Pass NULL in constructor if not using event queue.
         */
        ThreadSafeMessageQueue* m_pEvtQ;
        
        /**
        	List of online peers (unique peer id, unique peer name).
         */
        Peers peers_;
        
        /**
        	Current state of the GoCast::PeerConnectionClient instance.
         */
        State state_;
        
        bool m_bAudioOnly;
        int my_id_;
        
    protected:
        /**
        	Unique name of the peer client.
         */
        std::string m_PeerName;
        
        /**
            Public IP address of the signin server.
         */
        std::string m_ServerLocation;

        /**
        	Public port on which the signin server is listening.
         */
        int m_ServerPort;
    };
}
#endif
