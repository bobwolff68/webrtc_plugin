//
//  TestPeerConnectionClient.cpp
//  TestPeerConnectionClient
//
//  Created by Manjesh Malavalli on 10/14/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#include <iostream>
#include "TestPeerConnectionClient.h"
#include "TestPeerConnectionObserver.h"
#include "TestDefaults.h"
#include "talk/base/common.h"
#include "talk/base/nethelpers.h"
#include "talk/base/logging.h"
#include "talk/base/stringutils.h"

#ifdef WIN32
#include "talk/base/win32socketserver.h"
#endif

using talk_base::sprintfn;

namespace {
    
    // This is our magical hangup signal.
    const char kByeMessage[] = "BYE";
    
    talk_base::AsyncSocket* CreateClientSocket() {
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

TestPeerConnectionClient::TestPeerConnectionClient(): 
control_socket_(CreateClientSocket()),
hanging_get_(CreateClientSocket()),
state_(NOT_CONNECTED),
my_id_(-1) 
{
    control_socket_->SignalCloseEvent.connect(this,&TestPeerConnectionClient::OnClose);
    hanging_get_->SignalCloseEvent.connect(this,&TestPeerConnectionClient::OnClose);
    control_socket_->SignalConnectEvent.connect(this,&TestPeerConnectionClient::OnConnect);
    hanging_get_->SignalConnectEvent.connect(this,&TestPeerConnectionClient::OnHangingGetConnect);
    control_socket_->SignalReadEvent.connect(this,&TestPeerConnectionClient::OnRead);
    hanging_get_->SignalReadEvent.connect(this,&TestPeerConnectionClient::OnHangingGetRead);
}

TestPeerConnectionClient::~TestPeerConnectionClient()
{
}

int TestPeerConnectionClient::id() const 
{
    return my_id_;
}

bool TestPeerConnectionClient::is_connected() const 
{
    return my_id_ != -1;
}

const Peers& TestPeerConnectionClient::peers() const 
{
    return peers_;
}

bool TestPeerConnectionClient::Connect(const std::string& server, int port,
                                       const std::string& client_name) 
{
    ASSERT(!server.empty());
    ASSERT(!client_name.empty());
    
    if (state_ != NOT_CONNECTED) {
        LOG(WARNING)
        << "The client must not be connected before you can call Connect()";
        return false;
    }
    
    if (server.empty() || client_name.empty())
        return false;
    
    if (port <= 0)
        port = kDefaultServerPort;
    
    server_address_.SetIP(server);
    server_address_.SetPort(port);
    
    if (server_address_.IsUnresolved()) {
        int errcode = 0;
        hostent* h = talk_base::SafeGetHostByName(server_address_.IPAsString().c_str(), &errcode);
        if (!h) {
            LOG(LS_ERROR) << "Failed to resolve host name: "
            << server_address_.IPAsString();
            return false;
        } else {
            server_address_.SetResolvedIP(ntohl(*reinterpret_cast<uint32*>(h->h_addr_list[0])));
            talk_base::FreeHostEnt(h);
        }
    }
    
    char buffer[1024];
    sprintfn(buffer, sizeof(buffer),"GET /sign_in?%s HTTP/1.0\r\n\r\n", client_name.c_str());
    onconnect_data_ = buffer;
    
    bool ret = ConnectControlSocket();
    if (ret)
        state_ = SIGNING_IN;
    
    return ret;
}

bool TestPeerConnectionClient::SendToPeer(int peer_id, const std::string& message) 
{
    if (state_ != CONNECTED)
        return false;
    
    ASSERT(is_connected());
    //ASSERT(control_socket_->GetState() == talk_base::Socket::CS_CLOSED);
    if (!is_connected() || peer_id == -1)
        return false;
    
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

bool TestPeerConnectionClient::SendHangUp(int peer_id) 
{ 
    return SendToPeer(peer_id, kByeMessage);
}

bool TestPeerConnectionClient::IsSendingMessage()
{
    return state_ == CONNECTED &&
    control_socket_->GetState() != talk_base::Socket::CS_CLOSED;
}

bool TestPeerConnectionClient::SignOut() 
{
    if (state_ == NOT_CONNECTED || state_ == SIGNING_OUT)
        return true;
    
    if (hanging_get_->GetState() != talk_base::Socket::CS_CLOSED)
        hanging_get_->Close();
    
    if (control_socket_->GetState() == talk_base::Socket::CS_CLOSED) {
        state_ = SIGNING_OUT;
        
        if (my_id_ != -1) {
            char buffer[1024];
            sprintfn(buffer, sizeof(buffer),
                     "GET /sign_out?peer_id=%i HTTP/1.0\r\n\r\n", my_id_);
            onconnect_data_ = buffer;
            return ConnectControlSocket();
        } else {
            // Can occur if the app is closed before we finish connecting.
            return true;
        }
    } else {
        state_ = SIGNING_OUT_WAITING;
    }
    
    return true;
}

void TestPeerConnectionClient::Close() 
{
    control_socket_->Close();
    hanging_get_->Close();
    onconnect_data_.clear();
    peers_.clear();
    my_id_ = -1;
    state_ = NOT_CONNECTED;
}

bool TestPeerConnectionClient::ConnectControlSocket() 
{
    //ASSERT(control_socket_->GetState() == talk_base::Socket::CS_CLOSED);
    if(control_socket_->GetState() != talk_base::Socket::CS_CLOSED)
        return true;

    int err = control_socket_->Connect(server_address_);
    if (err == SOCKET_ERROR) {
        Close();
        return false;
    }
    return true;
}

void TestPeerConnectionClient::OnConnect(talk_base::AsyncSocket* socket)
{
    ASSERT(!onconnect_data_.empty());
    size_t sent = socket->Send(onconnect_data_.c_str(), onconnect_data_.length());
    ASSERT(sent == onconnect_data_.length());
    UNUSED(sent);
    onconnect_data_.clear();
}

void TestPeerConnectionClient::OnHangingGetConnect(talk_base::AsyncSocket* socket)
{
    char buffer[1024];
    sprintfn(buffer, sizeof(buffer),
             "GET /wait?peer_id=%i HTTP/1.0\r\n\r\n", my_id_);
    int len = strlen(buffer);
    int sent = socket->Send(buffer, len);
    ASSERT(sent == len);
    UNUSED2(sent, len);
}

void TestPeerConnectionClient::OnMessageFromPeer(int peer_id,const std::string& message)
{
    std::cout << "Peer(" << peer_id << "): " << message << std::endl;
    m_pObserver->OnMessageFromRemotePeer(peer_id, message);
}

bool TestPeerConnectionClient::GetHeaderValue(const std::string& data,size_t eoh,
                                              const char* header_pattern,
                                              size_t* value)
{
    ASSERT(value != NULL);
    size_t found = data.find(header_pattern);
    if (found != std::string::npos && found < eoh) {
        *value = atoi(&data[found + strlen(header_pattern)]);
        return true;
    }
    return false;
}

bool TestPeerConnectionClient::GetHeaderValue(const std::string& data, size_t eoh,
                                              const char* header_pattern,
                                              std::string* value)
{
    ASSERT(value != NULL);
    size_t found = data.find(header_pattern);
    if (found != std::string::npos && found < eoh) {
        size_t begin = found + strlen(header_pattern);
        size_t end = data.find("\r\n", begin);
        if (end == std::string::npos)
            end = eoh;
        value->assign(data.substr(begin, end - begin));
        return true;
    }
    return false;
}

bool TestPeerConnectionClient::ReadIntoBuffer(talk_base::AsyncSocket* socket,
                                              std::string* data,
                                              size_t* content_length)
{
    LOG(INFO) << __FUNCTION__;
    
    char buffer[0xffff];
    do {
        int bytes = socket->Recv(buffer, sizeof(buffer));
        if (bytes <= 0)
            break;
        data->append(buffer, bytes);
    } while (true);
    
    bool ret = false;
    size_t i = data->find("\r\n\r\n");
    if (i != std::string::npos) {
        LOG(INFO) << "Headers received";
        if (GetHeaderValue(*data, i, "\r\nContent-Length: ", content_length)) {
            LOG(INFO) << "Expecting " << *content_length << " bytes.";
            size_t total_response_size = (i + 4) + *content_length;
            if (data->length() >= total_response_size) {
                ret = true;
                std::string should_close;
                const char kConnection[] = "\r\nConnection: ";
                if (GetHeaderValue(*data, i, kConnection, &should_close) &&
                    should_close.compare("close") == 0) {
                    socket->Close();
                    // Since we closed the socket, there was no notification delivered
                    // to us.  Compensate by letting ourselves know.
                    OnClose(socket, 0);
                }
            } else {
                // We haven't received everything.  Just continue to accept data.
            }
        } else {
            LOG(LS_ERROR) << "No content length field specified by the server.";
        }
    }
    return ret;
}

void TestPeerConnectionClient::OnRead(talk_base::AsyncSocket* socket)
{
    LOG(INFO) << __FUNCTION__;
    size_t content_length = 0;
    if (ReadIntoBuffer(socket, &control_data_, &content_length)) {
        size_t peer_id = 0, eoh = 0;
        bool ok = ParseServerResponse(control_data_, content_length, &peer_id,
                                      &eoh);
        if (ok) {
            if (my_id_ == -1) {
                // First response.  Let's store our server assigned ID.
                ASSERT(state_ == SIGNING_IN);
                my_id_ = peer_id;
                ASSERT(my_id_ != -1);
                
                // The body of the response will be a list of already connected peers.
                if (content_length) {
                    size_t pos = eoh + 4;
                    while (pos < control_data_.size()) {
                        size_t eol = control_data_.find('\n', pos);
                        if (eol == std::string::npos)
                            break;
                        int id = 0;
                        std::string name;
                        bool connected;
                        if (ParseEntry(control_data_.substr(pos, eol - pos), &name, &id,
                                       &connected) && id != my_id_) {
                            peers_[id] = name;
                            std::cout << "PeerId(" << id << "): Online" << std::endl;
                        }
                        pos = eol + 1;
                    }
                }
                ASSERT(is_connected());
                std::cout << "Client: Sign in complete" << std::endl;
            } else if (state_ == SIGNING_OUT) {
                Close();
                std::cout << "Client: Signing out" << std::endl;
            } else if (state_ == SIGNING_OUT_WAITING) {
                SignOut();
                std::cout << "Client: Signing out waiting" << std::endl;
            }
        }
        
        control_data_.clear();
        
        if (state_ == SIGNING_IN) {
            ASSERT(hanging_get_->GetState() == talk_base::Socket::CS_CLOSED);
            state_ = CONNECTED;
            hanging_get_->Connect(server_address_);
        }
    }
}

void TestPeerConnectionClient::OnHangingGetRead(talk_base::AsyncSocket* socket)
{
    LOG(INFO) << __FUNCTION__;
    size_t content_length = 0;
    if (ReadIntoBuffer(socket, &notification_data_, &content_length)) {
        size_t peer_id = 0, eoh = 0;
        bool ok = ParseServerResponse(notification_data_, content_length,
                                      &peer_id, &eoh);
        
        if (ok) {
            // Store the position where the body begins.
            size_t pos = eoh + 4;
            
            if (my_id_ == static_cast<int>(peer_id)) {
                // A notification about a new member or a member that just
                // disconnected.
                int id = 0;
                std::string name;
                bool connected = false;
                if (ParseEntry(notification_data_.substr(pos), &name, &id,
                               &connected)) {
                    if (connected) {
                        peers_[id] = name;
                        std::cout << "Peer(" << id << "): Online" << std::endl;
                    } else {
                        peers_.erase(id);
                        std::cout << "Peer(" << id << "): Offline" << std::endl;
                    }
                }
            } else {
                OnMessageFromPeer(peer_id, notification_data_.substr(pos));
            }
        }
        
        notification_data_.clear();
    }
    
    if (hanging_get_->GetState() == talk_base::Socket::CS_CLOSED &&
        state_ == CONNECTED) {
        hanging_get_->Connect(server_address_);
    }
}

bool TestPeerConnectionClient::ParseEntry(const std::string& entry,
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
    if (separator != std::string::npos) {
        *id = atoi(&entry[separator + 1]);
        name->assign(entry.substr(0, separator));
        separator = entry.find(',', separator + 1);
        if (separator != std::string::npos) {
            *connected = atoi(&entry[separator + 1]) ? true : false;
        }
    }
    return !name->empty();
}

int TestPeerConnectionClient::GetResponseStatus(const std::string& response)
{
    int status = -1;
    size_t pos = response.find(' ');
    if (pos != std::string::npos)
        status = atoi(&response[pos + 1]);
    return status;
}

bool TestPeerConnectionClient::ParseServerResponse(const std::string& response,
                                                   size_t content_length,
                                                   size_t* peer_id,
                                                   size_t* eoh)
{
    LOG(INFO) << response;
    
    int status = GetResponseStatus(response.c_str());
    if (status != 200) {
        LOG(LS_ERROR) << "Received error from server";
        Close();
        return false;
    }
    
    *eoh = response.find("\r\n\r\n");
    ASSERT(*eoh != std::string::npos);
    if (*eoh == std::string::npos)
        return false;
    
    *peer_id = -1;
    
    // See comment in peer_channel.cc for why we use the Pragma header and
    // not e.g. "X-Peer-Id".
    GetHeaderValue(response, *eoh, "\r\nPragma: ", peer_id);
    
    return true;
}

void TestPeerConnectionClient::OnClose(talk_base::AsyncSocket* socket, int err)
{
    LOG(INFO) << __FUNCTION__;
    
    socket->Close();
    
#ifdef WIN32
    if (err != WSAECONNREFUSED) {
#else
    if (err != ECONNREFUSED) {
#endif
        if (socket == hanging_get_.get()) {
            if (state_ == CONNECTED) {
                LOG(INFO) << "Issuing  a new hanging get";
                hanging_get_->Close();
                hanging_get_->Connect(server_address_);
            }
        } else {
            //TODO: CONTROL SOCKET CLOSED -- DO SOMETHING
            ;
        }
    } else {
        LOG(WARNING) << "Failed to connect to the server";
        Close();
    }
}

void TestPeerConnectionClient::RegisterPeerConnectionObserver(TestPeerConnectionObserver *pObserver)
{
    m_pObserver = pObserver;
}

TestPeerConnectionObserver* TestPeerConnectionClient::GetPeerConnectionObserver(void) const
{
    return m_pObserver;
}
