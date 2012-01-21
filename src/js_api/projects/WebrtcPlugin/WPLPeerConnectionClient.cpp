/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

//  File: WPLPeerConnectionClient.cpp
//  Project: WebrtcPlugin

#include <iostream>
#include <unistd.h>
#include "WPLPeerConnectionClient.h"
#include "rtc_common.h"
#include "talk/base/common.h"
#include "talk/base/nethelpers.h"
#include "talk/base/logging.h"
#include "talk/base/stringutils.h"

#ifdef WIN32
#include "talk/base/win32socketserver.h"
#endif

using talk_base::sprintfn;

namespace
{
    talk_base::AsyncSocket* CreateClientSocket()
    {
    #ifdef WIN32
        return new talk_base::Win32Socket();
    #elif defined(POSIX)
        talk_base::Thread* thread = talk_base::Thread::Current();
        ASSERT(thread != NULL);
        return thread->socketserver()->CreateAsyncSocket(SOCK_STREAM);
    #else
        #error Platform not supported.
    #endif
    }    
}

namespace GoCast
{
    PeerConnectionClient::~PeerConnectionClient()
    {
        delete m_pCall;
    }

    int PeerConnectionClient::id() const 
    {
        return my_id_;
    }

    PeerConnectionClient::PeerConnectionClient(ThreadSafeMessageQueue* pMsgQ,
                                               ThreadSafeMessageQueue* pEvtQ,
                                               const std::string& peerName,
                                               const std::string& serverLocation,
                                               const int serverPort,
                                               const bool bAudioOnly):
    control_socket_(CreateClientSocket()),
    hanging_get_(CreateClientSocket()),
    m_pCall(new Call(pMsgQ,pEvtQ)),
    m_pMsgQ(pMsgQ),
    m_pEvtQ(pEvtQ),
    state_(NOT_CONNECTED),
    m_bAudioOnly(bAudioOnly),
    my_id_(-1),
    m_PeerName(peerName),
    m_ServerLocation(serverLocation),
    m_ServerPort(serverPort)
    {
        control_socket_->SignalCloseEvent.connect(this,&PeerConnectionClient::OnClose);
        hanging_get_->SignalCloseEvent.connect(this,&PeerConnectionClient::OnClose);
        control_socket_->SignalConnectEvent.connect(this,&PeerConnectionClient::OnConnect);
        hanging_get_->SignalConnectEvent.connect(this,&PeerConnectionClient::OnHangingGetConnect);
        control_socket_->SignalReadEvent.connect(this,&PeerConnectionClient::OnRead);
        hanging_get_->SignalReadEvent.connect(this,&PeerConnectionClient::OnHangingGetRead);
    }

    bool PeerConnectionClient::ExecuteNextCommand(bool& bQuitCommand)
    {
        bool bStatus = true;
        
        bQuitCommand = false;
        ParsedMessage cmd = m_pMsgQ->GetNextMessage();
        
        if(true == cmd["command"].empty())
        {
            return true;
        }
            
        if("signin" == cmd["command"] || "SIGNIN" == cmd["command"])
        {
            ASSERT(-1 == my_id_);
            ASSERT(NOT_CONNECTED == state_);

            if(NOT_CONNECTED != state_ || -1 < my_id_)
            {
                std::cout << "Error: Already signed in or in the process..." << std::endl;
                return false;
            }

            if(false == cmd["server"].empty())
            {
                m_ServerLocation = cmd["server"];
            }
            
            if(false == cmd["serverport"].empty())
            {
                sscanf(cmd["serverport"].c_str(),"%d",&m_ServerPort);
            }
            
            if(false == cmd["peername"].empty())
            {
                m_PeerName = cmd["peername"];
            }
            
            std::cout << std::endl << "Signing in..." << std::endl
                      << "Server: " << m_ServerLocation << std::endl
                      << "Port: " << m_ServerPort << std::endl;
            
            bStatus = Connect(m_ServerLocation, m_ServerPort, m_PeerName);
            
            if(false == bStatus)
            {
                std::cerr << __FUNCTION__ << ": Signin failed..." << std::endl;
            }
        }
        else if("signout" == cmd["command"] || "SIGNOUT" == cmd["command"])
        {
            ASSERT(-1 < my_id_);
            ASSERT(CONNECTED == state_);
            
            if(CONNECTED != state_ || -1 == my_id_)
            {
                std::cout << "Error: Already signed out or in the process..." << std::endl;
                return false;
            }
            
            if(true == m_pCall->IsActive())
            {
                std::cerr << __FUNCTION__ << ": Cannot sign out, call in progress..." << std::endl;
                return false;
            }
            
            bStatus = SignOut();
            
            if(false == bStatus)
            {
                std::cerr << __FUNCTION__ << ": Signout failed..." << std::endl;            
            }
        }
        else if("list" == cmd["command"] || "LIST" == cmd["command"])
        {
            std::cout << std::endl << "===== ONLINE PEERS =====" << std::endl;
            
            for(Peers::iterator it = peers_.begin();
                it != peers_.end();
                it++)
            {
                std::cout << it->second << std::endl;
            }
        }
        else if("calllist" == cmd["command"] || "CALLLIST" == cmd["command"])
        {
            m_pCall->ListParticipants();
        }
        else if("call" == cmd["command"] || "CALL" == cmd["command"])
        {
            ASSERT(CONNECTED == state_);
            ASSERT(-1 < my_id_);
            
            if(CONNECTED != state_ || -1 == my_id_)
            {
                std::cout << "Error: Cannot call - not signed in yet..." << std::endl;
                return false;
            }
            
            bStatus = false;
            cmd["peername"] = TOLOWERSTR(cmd["peername"]);
            
            
            if("" == cmd["credentials"])
            {
                for(Peers::iterator it = peers_.begin();
                    it != peers_.end();
                    it++)
                {
                    bStatus = (it->second == cmd["peername"]);
                    if(true == bStatus)
                    {
                        std::cout << std::endl << "Requesting peer: " << it->second << " for credentials..." << std::endl;
                        std::string credentials = "credentialsreq ";
                        credentials += (m_bAudioOnly ? "audioonly" : "audiovideo");
                        SendToPeer(it->first, credentials);                        
                        break;
                    }
                }
            }
            else
            {
                bool bAudioOnly = m_bAudioOnly || ("audioonly" == cmd["credentials"]);
                for(Peers::iterator it = peers_.begin();
                    it != peers_.end();
                    it++)
                {
                    bStatus = (it->second == cmd["peername"]);
                    if(true == bStatus)
                    {
                        std::cout << std::endl << "Calling peer: " << it->second << std::endl;

                        bool bStatus1 = m_pCall->AddParticipant(it->first, it->second,false,bAudioOnly);
                        
                        if(false == bStatus1)
                        {
                            std::cerr << __FUNCTION__ << ": Cannot call - peer already on call with you" << std::endl;
                            return false;
                        }
                        
                        break;
                    }
                }
            }
            
            if(false == bStatus)
            {
                std::cout << "Cannot call - peer " << cmd["peername"]
                          << " not online..." << std::endl;
            }
        }
        else if("sendtopeer" == cmd["command"] || "SENDTOPEER" == cmd["command"])
        {
            std::stringstream sstrm;
            int peerid;
            
            sstrm << cmd["peerid"];
            sstrm >> peerid;
            SendToPeer(peerid, cmd["message"]);
        }
        else if("hangup" == cmd["command"] || "HANGUP" == cmd["command"])
        {
            if(true == m_pCall->IsActive())
            {
                bStatus = false;            
                cmd["peername"] = TOLOWERSTR(cmd["peername"]);

                for(Peers::iterator it = peers_.begin();
                    it != peers_.end();
                    it++)
                {
                    if(cmd["peername"] == it->second)
                    {
                        std::cout << "Hanging up on " << it->second << "..." << std::endl;
                        bStatus = m_pCall->RemoveParticipant(it->first, false);
                        break;
                    }
                }
                
                if(false == bStatus)
                {
                    std::cerr << __FUNCTION__ << ": Hangup failed..." << std::endl;
                }
            }
            else
            {
                std::cerr << __FUNCTION__ << ": No active call to hang up..." << std::endl;
                return false;
            }
        }
        else if("deleteobserver" == cmd["command"] || "DELETEOBSERVER" == cmd["command"])
        {
            int peerId = FromString<int>(cmd["peerid"]);
            
            bStatus = m_pCall->RemoveParticipant(peerId, true);
            
            if(false == bStatus)
            {
                std::cerr << __FUNCTION__ << ": Failed to remove peer from call: " << peers_[peerId] << std::endl;
            }
        }
        else if("initpeerconnfactory" == cmd["command"] || "INITPEERCONNFACTORY" == cmd["command"])
        {
            bStatus = m_pCall->InitPeerConnectionFactory();
            
            if(false == bStatus)
            {
                std::cerr << __FUNCTION__ << ": Failed to init peerconnection factory" << std::endl;
            }
        }
        else if("deinitpeerconnfactory" == cmd["command"] || "DEINITPEERCONNFACTORY" == cmd["command"])
        {
            m_pCall->DeInitPeerConnectionFactory();
        }

#if(defined(GOCAST_ENABLE_VIDEO) && !defined(GOCAST_WINDOWS))
        else if("setremoterenderer" == cmd["command"] || "SETREMOTERENDERER" == cmd["command"])
        {
            int peerId = FromString<int>(cmd["peerid"]);
            
            bStatus = m_pCall->SetRemoteVideoRenderer(peerId, cmd["streamid"]);
        }
#endif

        else if("quit" == cmd["command"] || "QUIT" == cmd["command"] ||
                "exit" == cmd["command"] || "EXIT" == cmd["command"])
        {
            bQuitCommand = true;
            
            if(is_connected())
            {
                std::cerr << __FUNCTION__ << ": Cannot quit - still signed in..." << std::endl;
                bStatus = false;
            }
            else
            {
                bStatus = true;
            }
        }
            
        return bStatus;
    }

    bool PeerConnectionClient::is_connected() const 
    {
        return my_id_ != -1;
    }

    const Peers& PeerConnectionClient::peers() const 
    {
        return peers_;
    }

    bool PeerConnectionClient::Connect(const std::string& server, int port,
                                       const std::string& client_name) 
    {
        ASSERT(!server.empty());
        ASSERT(!client_name.empty());
        
        if (state_ != NOT_CONNECTED)
        {
            LOG(WARNING)
            << "The client must not be connected before you can call Connect()";
            return false;
        }
        
        if (server.empty() || client_name.empty())
        {
            return false;
        }
        
        if (port <= 0)
        {
            port = 8888;   
        }
        
        server_address_.SetIP(server);
        server_address_.SetPort(port);
        
        if (server_address_.IsUnresolved())
        {
            int errcode = 0;
            hostent* h = talk_base::SafeGetHostByName(server_address_.IPAsString().c_str(), &errcode);
            if (!h)
            {
                LOG(LS_ERROR) << "Failed to resolve host name: "
                << server_address_.IPAsString();
                return false;
            } else
            {
                server_address_.SetResolvedIP(ntohl(*reinterpret_cast<uint32*>(h->h_addr_list[0])));
                talk_base::FreeHostEnt(h);
            }
        }
        
        char buffer[1024];
        sprintfn(buffer, sizeof(buffer),"GET /sign_in?%s HTTP/1.0\r\n\r\n", client_name.c_str());
        onconnect_data_ = buffer;
        
        bool ret = ConnectControlSocket();
        if (ret)
        {
            state_ = SIGNING_IN;
        }
        
        return ret;
    }

    bool PeerConnectionClient::SendToPeer(int peer_id, const std::string& message) 
    {
        if (state_ != CONNECTED)
        {
            return false;
        }
        
        ASSERT(is_connected());
        if (!is_connected() || peer_id == -1)
        {
            return false;
        }
        
        char headers[1024];
        sprintfn(headers, sizeof(headers),
                 "POST /message?peer_id=%i&to=%i HTTP/1.0\r\n"
                 "Content-Length: %i\r\n"
                 "Content-Type: text/plain\r\n"
                 "\r\n",
                 my_id_, peer_id, message.length());
        onconnect_data_ = headers;
        onconnect_data_ += message;
        return ConnectControlSocket();
    }

    bool PeerConnectionClient::SignOut() 
    {
        if (state_ == NOT_CONNECTED || state_ == SIGNING_OUT)
        {
            return true;
        }
        
        if (hanging_get_->GetState() != talk_base::Socket::CS_CLOSED)
        {
            hanging_get_->Close();
        }
        
        if (control_socket_->GetState() == talk_base::Socket::CS_CLOSED)
        {
            state_ = SIGNING_OUT;
            
            if (my_id_ != -1)
            {
                char buffer[1024];
                sprintfn(buffer, sizeof(buffer),
                         "GET /sign_out?peer_id=%i HTTP/1.0\r\n\r\n", my_id_);
                onconnect_data_ = buffer;
                return ConnectControlSocket();
            } 
            else
            {
                // Can occur if the app is closed before we finish connecting.
                return true;
            }
        } 
        else
        {
            state_ = SIGNING_OUT_WAITING;
        }
        
        return true;
    }

    void PeerConnectionClient::Close() 
    {
        control_socket_->Close();
        hanging_get_->Close();
        onconnect_data_.clear();
        peers_.clear();
        my_id_ = -1;
        state_ = NOT_CONNECTED;
    }

    bool PeerConnectionClient::ConnectControlSocket() 
    {
        if(control_socket_->GetState() != talk_base::Socket::CS_CLOSED)
        {
            return true;
        }

        int err = control_socket_->Connect(server_address_);
        if (err == SOCKET_ERROR)
        {
            Close();
            return false;
        }
        
        return true;
    }

    void PeerConnectionClient::OnConnect(talk_base::AsyncSocket* socket)
    {
        ASSERT(!onconnect_data_.empty());
        size_t sent = socket->Send(onconnect_data_.c_str(), onconnect_data_.length());
        ASSERT(sent == onconnect_data_.length());
        
        if(onconnect_data_.length() > sent)
        {
            std::cerr << __FUNCTION__ << ": Failed to send to server: " << onconnect_data_ << std::endl;
        }
        
        UNUSED(sent);
        onconnect_data_.clear();
    }

    void PeerConnectionClient::OnHangingGetConnect(talk_base::AsyncSocket* socket)
    {
        char buffer[1024];
        sprintfn(buffer, sizeof(buffer),
                 "GET /wait?peer_id=%i HTTP/1.0\r\n\r\n", my_id_);
        int len = strlen(buffer);
        int sent = socket->Send(buffer, len);
        ASSERT(sent == len);
        UNUSED2(sent, len);
    }

    void PeerConnectionClient::OnMessageFromPeer(int peer_id,const std::string& message)
    {
        std::stringstream ss;
        std::string subject;
        std::string body;
        
        ss << message;
        ss >> subject;
        
        if("credentialsreq" == subject)
        {
            bool bAudioOnly = m_bAudioOnly;
            std::cout << peers_[peer_id] << " request: " << message << std::endl;
            ss >> body;
            bAudioOnly = bAudioOnly || ("audioonly" == body);
            
            ParsedMessage cmd;
            cmd["command"] = "sendtopeer";
            cmd["peerid"] = ToString(peer_id);
            cmd["message"] = "credentialsrep ";
            cmd["message"] += (m_bAudioOnly ? "audioonly" : "audiovideo");
            m_pMsgQ->PostMessage(cmd);
            
            if(false == m_pCall->HasParticipant(peer_id))
            {
                bool bStatus = m_pCall->AddParticipant(peer_id, peers_[peer_id], true, bAudioOnly);
                if(false == bStatus)
                {
                    std::cerr << __FUNCTION__ << ": Cannot add participant to call..." << std::endl;
                }
            }            
        }
        else if("credentialsrep" == subject)
        {
            std::cout << peers_[peer_id] << " reply: " << message << std::endl;
            ss >> body;
            
            ParsedMessage cmd;
            cmd["command"] = "call";
            cmd["peername"] = peers_[peer_id];
            cmd["credentials"] = body;
            m_pMsgQ->PostMessage(cmd);
        }
        else     
            m_pCall->OnMessageFromPeer(peer_id, message);
    }

    bool PeerConnectionClient::GetHeaderValue(const std::string& data,size_t eoh,
                                                  const char* header_pattern,
                                                  size_t* value)
    {
        ASSERT(value != NULL);
        size_t found = data.find(header_pattern);
        if (found != std::string::npos && found < eoh)
        {
            *value = atoi(&data[found + strlen(header_pattern)]);
            return true;
        }
        
        return false;
    }

    bool PeerConnectionClient::GetHeaderValue(const std::string& data, size_t eoh,
                                                  const char* header_pattern,
                                                  std::string* value)
    {
        ASSERT(value != NULL);
        size_t found = data.find(header_pattern);
        if (found != std::string::npos && found < eoh)
        {
            size_t begin = found + strlen(header_pattern);
            size_t end = data.find("\r\n", begin);
            
            if (end == std::string::npos)
            {
                end = eoh;
            }
            
            value->assign(data.substr(begin, end - begin));
            return true;
        }
        
        return false;
    }

    bool PeerConnectionClient::ReadIntoBuffer(talk_base::AsyncSocket* socket,
                                                  std::string* data,
                                                  size_t* content_length)
    {
        LOG(INFO) << __FUNCTION__;
        
        char buffer[0xffff];
        do
        {
            int bytes = socket->Recv(buffer, sizeof(buffer));
            if (bytes <= 0)
            {
                break;
            }
            
            data->append(buffer, bytes);
        } while (true);
        
        bool ret = false;
        size_t i = data->find("\r\n\r\n");
        if (i != std::string::npos)
        {
            LOG(INFO) << "Headers received";
            if (GetHeaderValue(*data, i, "\r\nContent-Length: ", content_length))
            {
                LOG(INFO) << "Expecting " << *content_length << " bytes.";
                size_t total_response_size = (i + 4) + *content_length;
                if (data->length() >= total_response_size)
                {
                    ret = true;
                    std::string should_close;
                    const char kConnection[] = "\r\nConnection: ";
                    if (GetHeaderValue(*data, i, kConnection, &should_close) &&
                        should_close.compare("close") == 0)
                    {
                        socket->Close();
                        // Since we closed the socket, there was no notification delivered
                        // to us.  Compensate by letting ourselves know.
                        OnClose(socket, 0);
                    }
                } 
                else
                {
                    ;// We haven't received everything.  Just continue to accept data.
                }
            } 
            else
            {
                LOG(LS_ERROR) << "No content length field specified by the server.";
            }
        }
        
        return ret;
    }

    void PeerConnectionClient::OnRead(talk_base::AsyncSocket* socket)
    {
        LOG(INFO) << __FUNCTION__;
        size_t content_length = 0;
        if (ReadIntoBuffer(socket, &control_data_, &content_length))
        {
            size_t peer_id = 0, eoh = 0;
            bool ok = ParseServerResponse(control_data_,
                                          content_length,
                                          &peer_id,
                                          &eoh);
            if (ok)
            {
                if (my_id_ == -1) 
                {
                    // First response.  Let's store our server assigned ID.
                    ASSERT(state_ == SIGNING_IN);
                    my_id_ = peer_id;
                    ASSERT(my_id_ != -1);
                    
                    // The body of the response will be a list of already connected peers.
                    if (content_length) 
                    {
                        size_t pos = eoh + 4;
                        while (pos < control_data_.size()) 
                        {
                            size_t eol = control_data_.find('\n', pos);
                            if (eol == std::string::npos)
                            {
                                break;
                            }
                            
                            int id = 0;
                            std::string name;
                            bool connected;
                            if (ParseEntry(control_data_.substr(pos, eol - pos), &name, &id,
                                           &connected) && id != my_id_) 
                            {
                                peers_[id] = name;
                                std::cout << "Peer["<< name << "] Online..." << std::endl;
                            }
                            
                            pos = eol + 1;
                        }
                    }
                    
                    ASSERT(is_connected());
                    std::cout << "Client: Sign in complete" << std::endl;
                    ParsedMessage cmd;
                    cmd["command"] = "initpeerconnfactory";
                    m_pMsgQ->PostMessage(cmd);
                    
                    if(NULL != m_pEvtQ)
                    {
                        ParsedMessage event;
                        event["type"] = "SignedIn";
                        
                        for(Peers::iterator it = peers_.begin();
                            it != peers_.end();
                            it++)
                        {
                            event["message"] += it->second;
                            event["message"] += ":";
                        }                    
                        
                        m_pEvtQ->PostMessage(event);
                    }
                }
                else if (state_ == SIGNING_OUT) 
                {
                    Close();
                    std::cout << "Client: Signing out" << std::endl;
                    ParsedMessage cmd;
                    cmd["command"] = "deinitpeerconnfactory";
                    m_pMsgQ->PostMessage(cmd);
                } 
                else if (state_ == SIGNING_OUT_WAITING) 
                {
                    SignOut();
                    std::cout << "Client: Signing out waiting" << std::endl;
                }
            }
            
            control_data_.clear();
            
            if (state_ == SIGNING_IN) 
            {
                ASSERT(hanging_get_->GetState() == talk_base::Socket::CS_CLOSED);
                state_ = CONNECTED;
                hanging_get_->Connect(server_address_);
            }
        }
    }

    void PeerConnectionClient::OnHangingGetRead(talk_base::AsyncSocket* socket)
    {
        LOG(INFO) << __FUNCTION__;
        size_t content_length = 0;
        if (ReadIntoBuffer(socket, &notification_data_, &content_length)) 
        {
            size_t peer_id = 0, eoh = 0;
            bool ok = ParseServerResponse(notification_data_, 
                                          content_length,
                                          &peer_id, &eoh);
            
            if (ok) 
            {
                // Store the position where the body begins.
                size_t pos = eoh + 4;
                
                if (my_id_ == static_cast<int>(peer_id)) 
                {
                    // A notification about a new member or a member that just
                    // disconnected.
                    int id = 0;
                    std::string name;
                    bool connected = false;
                    if (ParseEntry(notification_data_.substr(pos), &name, &id,
                                   &connected)) 
                    {
                        if (connected) 
                        {
                            peers_[id] = name;
                            std::cout << "Peer[" << name << "] Online..." << std::endl;
                            
                            if(NULL != m_pEvtQ)
                            {
                                ParsedMessage event;
                                event["type"] = "PeerOnline";
                                event["message"] = name;
                                m_pEvtQ->PostMessage(event);
                            }

                        } 
                        else 
                        {
                            peers_.erase(id);
                            std::cout << "Peer[" << name << "]: Offline..." << std::endl;
                            
                            if(NULL != m_pEvtQ)
                            {
                                ParsedMessage event;
                                event["type"] = "PeerOffline";
                                event["message"] = name;
                                m_pEvtQ->PostMessage(event);
                            }
                        }
                    }
                } 
                else 
                {
                    OnMessageFromPeer(peer_id, notification_data_.substr(pos));
                }
            }
            
            notification_data_.clear();
        }
        
        if (hanging_get_->GetState() == talk_base::Socket::CS_CLOSED &&
            state_ == CONNECTED) 
        {
            hanging_get_->Connect(server_address_);
        }
    }

    bool PeerConnectionClient::ParseEntry(const std::string& entry,
                                          std::string* name,
                                          int* id,
                                          bool* connected)
    {
        ASSERT(name != NULL);
        ASSERT(id != NULL);
        ASSERT(connected != NULL);
        ASSERT(!entry.empty());
        
        *connected = false;
        size_t separator = entry.find(',');
        if (separator != std::string::npos) 
        {
            *id = atoi(&entry[separator + 1]);
            name->assign(entry.substr(0, separator));
            separator = entry.find(',', separator + 1);
            if (separator != std::string::npos) 
            {
                *connected = atoi(&entry[separator + 1]) ? true : false;
            }
        }
        
        return !name->empty();
    }

    int PeerConnectionClient::GetResponseStatus(const std::string& response)
    {
        int status = -1;
        size_t pos = response.find(' ');
        if (pos != std::string::npos)
        {
            status = atoi(&response[pos + 1]);
        }
        
        return status;
    }

    bool PeerConnectionClient::ParseServerResponse(const std::string& response,
                                                       size_t content_length,
                                                       size_t* peer_id,
                                                       size_t* eoh)
    {
        LOG(INFO) << response;
        
        int status = GetResponseStatus(response.c_str());
        if (status != 200) 
        {
            LOG(LS_ERROR) << "Received error from server";
            Close();
            return false;
        }
        
        *eoh = response.find("\r\n\r\n");
        ASSERT(*eoh != std::string::npos);
        if (*eoh == std::string::npos)
        {    
            return false;
        }
        
        *peer_id = -1;
        
        // See comment in peer_channel.cc for why we use the Pragma header and
        // not e.g. "X-Peer-Id".
        GetHeaderValue(response, *eoh, "\r\nPragma: ", peer_id);
        
        return true;
    }

    void PeerConnectionClient::OnClose(talk_base::AsyncSocket* socket, int err)
    {
        LOG(INFO) << __FUNCTION__;
        
        socket->Close();
        
    #ifdef WIN32
        if (err != WSAECONNREFUSED) 
        {
    #else
        if (err != ECONNREFUSED) 
        {
    #endif
            if (socket == hanging_get_.get()) 
            {
                if (state_ == CONNECTED) {
                    LOG(INFO) << "Issuing  a new hanging get";
                    hanging_get_->Close();
                    hanging_get_->Connect(server_address_);
                }
            } 
            else 
            {
                //TODO: CONTROL SOCKET CLOSED -- DO SOMETHING
                ;
            }
        } 
        else 
        {
            LOG(WARNING) << "Failed to connect to the server";
            Close();
        }
    }
        
    bool PeerConnectionClient::InitPeerConnectionFactory() {
        return m_pCall->InitPeerConnectionFactory();
    }
        
    void PeerConnectionClient::DeInitPeerConnectionFactory() {
        m_pCall->DeInitPeerConnectionFactory();    
    }
}
