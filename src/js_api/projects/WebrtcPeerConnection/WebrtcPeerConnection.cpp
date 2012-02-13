/**********************************************************\

  Auto-generated WebrtcPeerConnection.cpp

  This file contains the auto-generated main plugin object
  implementation for the WebrtcPeerConnection project

\**********************************************************/

#include "WebrtcPeerConnectionAPI.h"

#include "WebrtcPeerConnection.h"

#include "talk/base/common.h"
#include "talk/session/phone/mediaengine.h"
#include "talk/session/phone/webrtcvoiceengine.h"
#include "talk/p2p/client/basicportallocator.h"


talk_base::scoped_ptr<webrtc::PeerConnectionFactory> WebrtcPeerConnection::m_pPeerConnectionFactory;
talk_base::scoped_ptr<talk_base::Thread> WebrtcPeerConnection::m_pWorkerThread;
cricket::MediaEngineInterface* WebrtcPeerConnection::m_pMediaEngine = NULL;
cricket::DeviceManagerInterface* WebrtcPeerConnection::m_pDeviceManager = NULL;

///////////////////////////////////////////////////////////////////////////////
/// @fn WebrtcPeerConnection::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginInitialize()
///
/// @see FB::FactoryBase::globalPluginInitialize
///////////////////////////////////////////////////////////////////////////////
void WebrtcPeerConnection::StaticInitialize()
{
    // Place one-time initialization stuff here; As of FireBreath 1.4 this should only
    // be called once per process
    if(NULL==m_pPeerConnectionFactory.get() && NULL==m_pWorkerThread.get())
    {
        //Start factory worker thread
        m_pWorkerThread.reset(new talk_base::Thread());
        if(false == m_pWorkerThread->SetName("FactoryWT", NULL) ||
           false == m_pWorkerThread->Start())
        {
            m_pWorkerThread.reset();
            return;
        }
        
        //Create port allocator of peer connection factory
        cricket::PortAllocator* pPortAllocator = NULL;
        pPortAllocator = new cricket::BasicPortAllocator(
                            new talk_base::BasicNetworkManager(),
                            talk_base::SocketAddress("stun.l.google.com", 19302),
                            talk_base::SocketAddress(),
                            talk_base::SocketAddress(),
                            talk_base::SocketAddress()
                        );
        
        //Create media engine and device manager
        m_pMediaEngine = new cricket::CompositeMediaEngine<cricket::WebRtcVoiceEngine,cricket::NullVideoEngine>();
        m_pDeviceManager = new cricket::DeviceManager();
        
        //Create peer connection factory
        m_pPeerConnectionFactory.reset(
            new webrtc::PeerConnectionFactory(
                pPortAllocator,
                m_pMediaEngine,
                m_pDeviceManager,
                m_pWorkerThread.get()
            )
        );
        
        if(false == m_pPeerConnectionFactory->Initialize())
        {
            m_pPeerConnectionFactory.reset();
            m_pWorkerThread.reset();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @fn WebrtcPeerConnection::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginDeinitialize()
///
/// @see FB::FactoryBase::globalPluginDeinitialize
///////////////////////////////////////////////////////////////////////////////
void WebrtcPeerConnection::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here. As of FireBreath 1.4 this should
    // always be called just before the plugin library is unloaded
    m_pPeerConnectionFactory.reset();
    m_pWorkerThread.reset();
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  WebrtcPeerConnection constructor.  Note that your API is not available
///         at this point, nor the window.  For best results wait to use
///         the JSAPI object until the onPluginReady method is called
///////////////////////////////////////////////////////////////////////////////
WebrtcPeerConnection::WebrtcPeerConnection()
{
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  WebrtcPeerConnection destructor.
///////////////////////////////////////////////////////////////////////////////
WebrtcPeerConnection::~WebrtcPeerConnection()
{
    // This is optional, but if you reset m_api (the shared_ptr to your JSAPI
    // root object) and tell the host to free the retained JSAPI objects then
    // unless you are holding another shared_ptr reference to your JSAPI object
    // they will be released here.
    releaseRootJSAPI();
    m_host->freeRetainedObjects();
}

void WebrtcPeerConnection::onPluginReady()
{
    // When this is called, the BrowserHost is attached, the JSAPI object is
    // created, and we are ready to interact with the page and such.  The
    // PluginWindow may or may not have already fire the AttachedEvent at
    // this point.
}

void WebrtcPeerConnection::shutdown()
{
    // This will be called when it is time for the plugin to shut down;
    // any threads or anything else that may hold a shared_ptr to this
    // object should be released here so that this object can be safely
    // destroyed. This is the last point that shared_from_this and weak_ptr
    // references to this object will be valid
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Creates an instance of the JSAPI object that provides your main
///         Javascript interface.
///
/// Note that m_host is your BrowserHost and shared_ptr returns a
/// FB::PluginCorePtr, which can be used to provide a
/// boost::weak_ptr<WebrtcPeerConnection> for your JSAPI class.
///
/// Be very careful where you hold a shared_ptr to your plugin class from,
/// as it could prevent your plugin class from getting destroyed properly.
///////////////////////////////////////////////////////////////////////////////
FB::JSAPIPtr WebrtcPeerConnection::createJSAPI()
{
    // m_host is the BrowserHost
    return boost::make_shared<WebrtcPeerConnectionAPI>(FB::ptr_cast<WebrtcPeerConnection>(shared_from_this()), m_host, &m_pPeerConnectionFactory, &m_pWorkerThread);
}

bool WebrtcPeerConnection::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool WebrtcPeerConnection::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool WebrtcPeerConnection::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}
bool WebrtcPeerConnection::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *)
{
    // The window is attached; act appropriately
    return false;
}

bool WebrtcPeerConnection::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *)
{
    // The window is about to be detached; act appropriately
    return false;
}

