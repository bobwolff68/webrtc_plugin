/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

/**********************************************************\

  Auto-generated WebrtcPluginAPI.cpp

\**********************************************************/

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"

#include "WebrtcPluginAPI.h"
#include "WPLPeerConnectionClient.h"
#include "WPLSocketServer.h"
#include "talk/base/thread.h"
#include "rtc_common.h"

int PluginMainThread::workerBee(void)
{
    talk_base::AutoThread autoThread;

    GoCast::SocketServer socketServer;
    talk_base::Thread::Current()->set_socketserver(&socketServer);
    
    GoCast::PeerConnectionClient testClient(m_pMsgQ, m_pEvtQ, "", "", -1, m_bAudioOnly);
    socketServer.SetPeerConnectionClient(&testClient);

    talk_base::Thread::Current()->Run();
    talk_base::Thread::Current()->set_socketserver(NULL);
    return 0;
}

int PluginNotificationsThread::workerBee(void)
{
    std::string eventType = "";
    do
    {
        ParsedMessage event = m_pEvtQ->GetNextMessage();
        eventType = event["type"];
        
        if("SignedIn" == eventType)
        {
            m_pPluginAPI->fire_SignedIn(event["message"]);
        }
        else if("PeerOnline" == eventType)
        {
            m_pPluginAPI->fire_PeerOnline(event["message"]);
        }
        else if("PeerOffline" == eventType)
        {
            m_pPluginAPI->fire_PeerOffline(event["message"]);
        }
        else if("RemotePeerCall" == eventType)
        {
            m_pPluginAPI->fire_RemotePeerCall(event["message"]);
        }
        else if("RemotePeerHangup" == eventType)
        {
            m_pPluginAPI->fire_RemotePeerHangup(event["message"]);
        }
        else if("RendererAdd" == eventType)
        {
            m_pPluginAPI->fire_RendererAdd(event["message"]);
        }
        else if("RendererRemove" == eventType)
        {
            m_pPluginAPI->fire_RendererRemove(event["message"]);
        }
        
        usleep(100000);
    } while(eventType != "quit");
            
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn WebrtcPluginAPI::WebrtcPluginAPI(const WebrtcPluginPtr& plugin, const FB::BrowserHostPtr host)
///
/// @brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @see FB::JSAPIAuto::registerMethod
/// @see FB::JSAPIAuto::registerProperty
/// @see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
WebrtcPluginAPI::WebrtcPluginAPI(const WebrtcPluginPtr& plugin, const FB::BrowserHostPtr& host) : m_plugin(plugin), m_host(host)
{
    registerMethod("echo",      make_method(this, &WebrtcPluginAPI::echo));
    registerMethod("testEvent", make_method(this, &WebrtcPluginAPI::testEvent));
    registerMethod("Signin", make_method(this, &WebrtcPluginAPI::Signin));
    registerMethod("Signout", make_method(this, &WebrtcPluginAPI::Signout));
    registerMethod("Call", make_method(this, &WebrtcPluginAPI::Call));
    registerMethod("Hangup", make_method(this, &WebrtcPluginAPI::Hangup));
    registerMethod("Start", make_method(this, &WebrtcPluginAPI::Start));
    
    // Read-write property
    registerProperty("testString",
                     make_property(this,
                        &WebrtcPluginAPI::get_testString,
                        &WebrtcPluginAPI::set_testString));

    // Read-only property
    registerProperty("version",
                     make_property(this,
                        &WebrtcPluginAPI::get_version));
    
    m_testString = "Hello World";
    
    m_pMsgQ = new (GoCast::ThreadSafeMessageQueue)();
    m_pEvtQ = new (GoCast::ThreadSafeMessageQueue)();
}

///////////////////////////////////////////////////////////////////////////////
/// @fn WebrtcPluginAPI::~WebrtcPluginAPI()
///
/// @brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
WebrtcPluginAPI::~WebrtcPluginAPI()
{
    ParsedMessage cmd;
    ParsedMessage event;
    
    cmd["command"] = "quit";
    m_pMsgQ->PostMessage(cmd);
    m_pMainThread->stopThread();
    
    event["type"] = "quit";
    m_pEvtQ->PostMessage(event);
    m_pNotificationsThread->stopThread();
    
    delete m_pMsgQ;
    delete m_pEvtQ;
    delete m_pMainThread;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn WebrtcPluginPtr WebrtcPluginAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
WebrtcPluginPtr WebrtcPluginAPI::getPlugin()
{
    WebrtcPluginPtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}



// Read/Write property testString
std::string WebrtcPluginAPI::get_testString()
{
    return m_testString;
}
void WebrtcPluginAPI::set_testString(const std::string& val)
{
    m_testString = val;
}

// Read-only property version
std::string WebrtcPluginAPI::get_version()
{
    return "CURRENT_VERSION";
}

// Method echo
FB::variant WebrtcPluginAPI::echo(const FB::variant& msg)
{
    static int n(0);
    fire_echo(msg, n++);
    return msg;
}

void WebrtcPluginAPI::testEvent(const FB::variant& var)
{
    fire_fired(var, true, 1);
}

void WebrtcPluginAPI::Signin(const std::string& peerName,
                             const std::string& serverIP,
                             const int serverPort)
{
    ParsedMessage cmd;

    cmd["command"] = "signin";
    cmd["server"] = serverIP,
    cmd["peername"] = peerName,
    cmd["serverport"] = ::ToString(serverPort);
    m_pMsgQ->PostMessage(cmd);
}

void WebrtcPluginAPI::Signout(void)
{
    ParsedMessage cmd;
    
    cmd["command"] = "signout";
    m_pMsgQ->PostMessage(cmd);
}

void WebrtcPluginAPI::Call(const std::string& peerName)
{
    ParsedMessage cmd;
    
    cmd["command"] = "call";
    cmd["peername"] = peerName;
    m_pMsgQ->PostMessage(cmd);
}

void WebrtcPluginAPI::Hangup(const std::string& peerName)
{
    ParsedMessage cmd;
    
    cmd["command"] = "hangup";
    cmd["peername"] = peerName;
    m_pMsgQ->PostMessage(cmd);
}

void WebrtcPluginAPI::Start(const std::string& mediaType)
{
    bool bAudioOnly = ("audioonly" == mediaType) ? true : false;
    m_pMainThread = new PluginMainThread(m_pMsgQ, m_pEvtQ, bAudioOnly);
    m_pNotificationsThread = new PluginNotificationsThread(this, m_pEvtQ);
    m_pMainThread->startThread();
    m_pNotificationsThread->startThread();
}
