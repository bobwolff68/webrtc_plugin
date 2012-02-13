/**********************************************************\

  Auto-generated WebrtcPeerConnection.h

  This file contains the auto-generated main plugin object
  implementation for the WebrtcPeerConnection project

\**********************************************************/
#ifndef H_WebrtcPeerConnectionPLUGIN
#define H_WebrtcPeerConnectionPLUGIN

#include "PluginWindow.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/AttachedEvent.h"

#include "PluginCore.h"

#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/base/scoped_ptr.h"

FB_FORWARD_PTR(WebrtcPeerConnection)
class WebrtcPeerConnection : public FB::PluginCore
{
public:
    static void StaticInitialize();
    static void StaticDeinitialize();

public:
    WebrtcPeerConnection();
    virtual ~WebrtcPeerConnection();

public:
    void onPluginReady();
    void shutdown();
    virtual FB::JSAPIPtr createJSAPI();
    // If you want your plugin to always be windowless, set this to true
    // If you want your plugin to be optionally windowless based on the
    // value of the "windowless" param tag, remove this method or return
    // FB::PluginCore::isWindowless()
    virtual bool isWindowless() { return false; }

    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::AttachedEvent, onWindowAttached, FB::PluginWindow)
        EVENTTYPE_CASE(FB::DetachedEvent, onWindowDetached, FB::PluginWindow)
    END_PLUGIN_EVENT_MAP()

    /** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
    virtual bool onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *);
    virtual bool onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *);
    virtual bool onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *);
    virtual bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *);
    virtual bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *);
    /** END EVENTDEF -- DON'T CHANGE THIS LINE **/
    
private:
    static talk_base::scoped_ptr<webrtc::PeerConnectionFactory> m_pPeerConnectionFactory;
    static talk_base::scoped_ptr<talk_base::Thread> m_pWorkerThread;
    static cricket::MediaEngineInterface* m_pMediaEngine;
    static cricket::DeviceManagerInterface* m_pDeviceManager;
};


#endif

