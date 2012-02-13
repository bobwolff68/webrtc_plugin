/**********************************************************\

  Auto-generated WebrtcPeerConnectionAPI.h

\**********************************************************/

#include <string>
#include <sstream>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "WebrtcPeerConnection.h"

#include "talk/app/webrtc/peerconnection.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/base/scoped_ptr.h"

#ifndef H_WebrtcPeerConnectionAPI
#define H_WebrtcPeerConnectionAPI

class WebrtcPeerConnectionAPI : public FB::JSAPIAuto, public webrtc::PeerConnectionObserver
{
public:
    WebrtcPeerConnectionAPI(const WebrtcPeerConnectionPtr& plugin,
                            const FB::BrowserHostPtr& host,
                            talk_base::scoped_ptr<webrtc::PeerConnectionFactory>* pFactory,
                            talk_base::scoped_ptr<talk_base::Thread>* pFactoryWorkerThread);
    virtual ~WebrtcPeerConnectionAPI();

    WebrtcPeerConnectionPtr getPlugin();

    //--------------------- Plugin Properties ----------------------
    //logCallback
    FB::JSObjectPtr get_logCallback();
    void set_logCallback(const FB::JSObjectPtr& pJSCallback);

    //onAddStreamCallback
    FB::JSObjectPtr get_onAddStreamCallback();
    void set_onAddStreamCallback(const FB::JSObjectPtr& pJSCallback);

    //onRemoveStreamCallback
    FB::JSObjectPtr get_onRemoveStreamCallback();
    void set_onRemoveStreamCallback(const FB::JSObjectPtr& pJSCallback);

    //onSignalingMessageCallback
    FB::JSObjectPtr get_onSignalingMessageCallback();
    void set_onSignalingMessageCallback(const FB::JSObjectPtr& pJSCallback);

    //readyState
    FB::variant get_readyState();
    
    //--------------------- Plugin methods -------------------------
    FB::variant Init();
    FB::variant Connect();
    FB::variant AddStream(const std::string& streamId, bool bVideo);
    FB::variant RemoveStream(const std::string& streamId);
    FB::variant ProcessSignalingMessage(const std::string& message);
    FB::variant Close();
    
    //-------------- PeerConnection Observer Methods -----------------------
    virtual void OnAddStream(const std::string& streamId, bool bVideo);
    virtual void OnRemoveStream(const std::string& streamId, bool bVideo);
    virtual void OnSignalingMessage(const std::string& message);
    
    
private:
    WebrtcPeerConnectionWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;
    talk_base::scoped_ptr<webrtc::PeerConnectionFactory>* m_pFactory;
    talk_base::scoped_ptr<talk_base::Thread>* m_pFactoryWorkerThread;

    //Properties
    webrtc::PeerConnection::ReadyState m_readyState;
    
    //JS Callbacks
    FB::JSObjectPtr m_logCallback;
    FB::JSObjectPtr m_onAddStreamCallback;
    FB::JSObjectPtr m_onRemoveStreamCallback;
    FB::JSObjectPtr m_onSignalingMessageCallback;
    
    talk_base::scoped_ptr<webrtc::PeerConnection> m_pPeerConnection;
};

#endif // H_WebrtcPeerConnectionAPI

