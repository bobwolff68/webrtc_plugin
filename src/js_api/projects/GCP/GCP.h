/**********************************************************\

  Auto-generated GCP.h

  This file contains the auto-generated main plugin object
  implementation for the GoCastPlayer project

\**********************************************************/
#ifndef H_GCPPLUGIN
#define H_GCPPLUGIN

#include "PluginWindow.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/AttachedEvent.h"

#include "PluginCore.h"

#include <map>
#include <deque>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

//#include "talk/app/webrtc/mediastreaminterface.h"
//#include "talk/base/scoped_ref_ptr.h"
#include "GCPVideoRenderer.h"

FB_FORWARD_PTR(GCP)
class GCP : public FB::PluginCore
{
public:
    static void StaticInitialize();
    static void StaticDeinitialize();
    
public:
    GCP();
    virtual ~GCP();    
    GoCast::GCPVideoRenderer* Renderer() { return m_pRenderer; }
    
public:
    void onPluginReady();
    void shutdown();
    virtual FB::JSAPIPtr createJSAPI();
    // If you want your plugin to always be windowless, set this to true
    // If you want your plugin to be optionally windowless based on the
    // value of the "windowless" param tag, remove this method or return
    // FB::PluginCore::isWindowless()
#ifdef GOCAST_WINDOWS
    virtual bool isWindowless() { return true; }
#endif

    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::AttachedEvent, onWindowAttached, FB::PluginWindow)
        EVENTTYPE_CASE(FB::DetachedEvent, onWindowDetached, FB::PluginWindow)
        EVENTTYPE_CASE(FB::RefreshEvent, onWindowRefresh, FB::PluginWindow)
    END_PLUGIN_EVENT_MAP()

    /** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
    virtual bool onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *);
    virtual bool onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *);
    virtual bool onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *);
    virtual bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *);
    virtual bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *);    
    virtual bool onWindowRefresh(FB::RefreshEvent *evt, FB::PluginWindow *);
    /** END EVENTDEF -- DON'T CHANGE THIS LINE **/
    
private:
    GoCast::GCPVideoRenderer* m_pRenderer;
};

#endif

