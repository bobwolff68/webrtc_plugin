/**********************************************************\

  Auto-generated WebrtcPeerConnectionAPI.cpp

\**********************************************************/

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"

#include "WebrtcPeerConnectionAPI.h"
#include "talk/session/phone/webrtcvoiceengine.h"

#define GOCAST_AUDIO_OPTS (cricket::MediaEngineInterface::ECHO_CANCELLATION |\
cricket::MediaEngineInterface::NOISE_SUPPRESSION |\
cricket::MediaEngineInterface::AUTO_GAIN_CONTROL)

///////////////////////////////////////////////////////////////////////////////
/// @fn WebrtcPeerConnectionAPI::WebrtcPeerConnectionAPI(const WebrtcPeerConnectionPtr& plugin, const FB::BrowserHostPtr host)
///
/// @brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @see FB::JSAPIAuto::registerMethod
/// @see FB::JSAPIAuto::registerProperty
/// @see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
WebrtcPeerConnectionAPI::WebrtcPeerConnectionAPI(const WebrtcPeerConnectionPtr& plugin,
                                                 const FB::BrowserHostPtr& host,
                                                 talk_base::scoped_ptr<webrtc::PeerConnectionFactory>* pFactory,
                                                 talk_base::scoped_ptr<talk_base::Thread>* pFactoryWorkerThread
                                                 )
    : m_plugin(plugin)
    , m_host(host)
    , m_pFactory(pFactory)
    , m_pFactoryWorkerThread(pFactoryWorkerThread)
{
    registerMethod("addStream", make_method(this, &WebrtcPeerConnectionAPI::AddStream));
    registerMethod("removeStream", make_method(this, &WebrtcPeerConnectionAPI::RemoveStream));
    registerMethod("processSignalingMessage", make_method(this, &WebrtcPeerConnectionAPI::ProcessSignalingMessage));
    registerMethod("close", make_method(this, &WebrtcPeerConnectionAPI::Close));
    registerMethod("init", make_method(this, &WebrtcPeerConnectionAPI::Init));
    registerMethod("connect", make_method(this, &WebrtcPeerConnectionAPI::Connect));
    
    registerProperty("readyState",make_property(this,&WebrtcPeerConnectionAPI::get_readyState));
    registerProperty("onlogmessage",make_property(this,&WebrtcPeerConnectionAPI::get_logCallback,
                                                  &WebrtcPeerConnectionAPI::set_logCallback));
    registerProperty("onaddstream",make_property(this,&WebrtcPeerConnectionAPI::get_onAddStreamCallback,
                                                 &WebrtcPeerConnectionAPI::set_onAddStreamCallback));
    registerProperty("onremovestream",make_property(this,&WebrtcPeerConnectionAPI::get_onRemoveStreamCallback,
                                                    &WebrtcPeerConnectionAPI::set_onRemoveStreamCallback));
    registerProperty("onsignalingmessage",make_property(this,&WebrtcPeerConnectionAPI::get_onSignalingMessageCallback,
                                                        &WebrtcPeerConnectionAPI::set_onSignalingMessageCallback));
}

///////////////////////////////////////////////////////////////////////////////
/// @fn WebrtcPeerConnectionAPI::~WebrtcPeerConnectionAPI()
///
/// @brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
WebrtcPeerConnectionAPI::~WebrtcPeerConnectionAPI()
{
    m_pPeerConnection.reset();
}

///////////////////////////////////////////////////////////////////////////////
/// @fn WebrtcPeerConnectionPtr WebrtcPeerConnectionAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
WebrtcPeerConnectionPtr WebrtcPeerConnectionAPI::getPlugin()
{
    WebrtcPeerConnectionPtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}

FB::variant WebrtcPeerConnectionAPI::get_readyState()
{
    FB::variant readyState = m_readyState;
    return readyState;
}

FB::JSObjectPtr WebrtcPeerConnectionAPI::get_logCallback()
{
    return m_logCallback;
}

void WebrtcPeerConnectionAPI::set_logCallback(const FB::JSObjectPtr& pJSCallback)
{
    m_logCallback = pJSCallback;
}

FB::JSObjectPtr WebrtcPeerConnectionAPI::get_onAddStreamCallback()
{
    return m_onAddStreamCallback;
}

void WebrtcPeerConnectionAPI::set_onAddStreamCallback(const FB::JSObjectPtr& pJSCallback)
{
    m_onAddStreamCallback = pJSCallback;
}

FB::JSObjectPtr WebrtcPeerConnectionAPI::get_onRemoveStreamCallback()
{
    return m_onRemoveStreamCallback;
}

void WebrtcPeerConnectionAPI::set_onRemoveStreamCallback(const FB::JSObjectPtr& pJSCallback)
{
    m_onRemoveStreamCallback = pJSCallback;
}

FB::JSObjectPtr WebrtcPeerConnectionAPI::get_onSignalingMessageCallback()
{
    return m_onSignalingMessageCallback;
}

void WebrtcPeerConnectionAPI::set_onSignalingMessageCallback(const FB::JSObjectPtr& pJSCallback)
{
    m_onSignalingMessageCallback = pJSCallback;
}

void WebrtcPeerConnectionAPI::OnAddStream(const std::string& streamId, bool bVideo)
{
    m_onAddStreamCallback->InvokeAsync("", FB::variant_list_of(streamId)(bVideo));
}

void WebrtcPeerConnectionAPI::OnRemoveStream(const std::string& streamId, bool bVideo)
{
    m_onRemoveStreamCallback->InvokeAsync("", FB::variant_list_of(streamId)(bVideo));
}

void WebrtcPeerConnectionAPI::OnSignalingMessage(const std::string& message)
{
    m_onSignalingMessageCallback->InvokeAsync("", FB::variant_list_of(message));
}

FB::variant WebrtcPeerConnectionAPI::AddStream(const std::string& streamId, bool bVideo)
{
    FB::variant ret = m_pPeerConnection->AddStream(streamId, bVideo);
    
    std::string logMsg = "AddStream(";
    logMsg += streamId;
    logMsg += ",";
    logMsg += (bVideo?"true":"false");
    logMsg += "): ==> ";
    logMsg += (ret.convert_cast<bool>()?"successful":"failed"); 
    m_logCallback->InvokeAsync("", FB::variant_list_of(logMsg));
                               
    return ret;
}

FB::variant WebrtcPeerConnectionAPI::RemoveStream(const std::string& streamId)
{
    FB::variant ret = m_pPeerConnection->RemoveStream(streamId);

    std::string logMsg = "RemoveStream(";
    logMsg += streamId;
    logMsg += "): ==> ";
    logMsg += (ret.convert_cast<bool>()?"successful":"failed"); 
    m_logCallback->InvokeAsync("", FB::variant_list_of(logMsg));
    
    return ret;
}

FB::variant WebrtcPeerConnectionAPI::ProcessSignalingMessage(const std::string& message)
{
    FB::variant ret = m_pPeerConnection->SignalingMessage(message);

    std::string logMsg = "ProcessSignalingMessage(message): ==> ";
    logMsg += (ret.convert_cast<bool>()?"successful":"failed"); 
    m_logCallback->InvokeAsync("", FB::variant_list_of(logMsg));
    
    return ret;
}

FB::variant WebrtcPeerConnectionAPI::Close()
{
    FB::variant ret = m_pPeerConnection->Close();

    std::string logMsg = "Close(): ==> ";
    logMsg += (ret.convert_cast<bool>()?"successful":"failed"); 
    m_logCallback->InvokeAsync("", FB::variant_list_of(logMsg));
    
    return ret;
}

FB::variant WebrtcPeerConnectionAPI::Init()
{
    FB::variant ret = true;
    std::string logMsg = "Init(): ==> ";
    
    if(NULL != m_pPeerConnection.get())
    {
        logMsg += "successful(peerconnection already init'ed)";
        m_logCallback->InvokeAsync("", FB::variant_list_of(logMsg));
        return ret;
    }
    
    m_pPeerConnection.reset((*m_pFactory)->CreatePeerConnection(m_pFactoryWorkerThread->get()));
    if(NULL == m_pPeerConnection.get())
    {
        ret = false;
        logMsg += "failed(couldn't create peerconnection object)";
        m_logCallback->InvokeAsync("", FB::variant_list_of(logMsg));
        return ret;
    }
    
    m_pPeerConnection->RegisterObserver(this);
    
    if(false == m_pPeerConnection->SetAudioDevice("", "", GOCAST_AUDIO_OPTS))
    {
        m_pPeerConnection.reset();
        ret = false;
        logMsg += "failed(couldn't set audio device)";
        m_logCallback->InvokeAsync("", FB::variant_list_of(logMsg));
        return ret;
    }
    
    logMsg += "successful";
    m_logCallback->InvokeAsync("", FB::variant_list_of(logMsg));
    return ret;
}

FB::variant WebrtcPeerConnectionAPI::Connect()
{
    FB::variant ret = m_pPeerConnection->Connect();
    
    std::string logMsg = "Connect(): ==> ";
    logMsg += (ret.convert_cast<bool>()?"successful":"failed"); 
    m_logCallback->InvokeAsync("", FB::variant_list_of(logMsg));

    return ret;
}
