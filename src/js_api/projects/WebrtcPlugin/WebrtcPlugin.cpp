/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

/**********************************************************\

  Auto-generated WebrtcPlugin.cpp

  This file contains the auto-generated main plugin object
  implementation for the WebrtcPlugin project

\**********************************************************/

#include "WebrtcPluginAPI.h"

#include "WebrtcPlugin.h"

FB::PluginWindow* pThePluginWindow = NULL;

///////////////////////////////////////////////////////////////////////////////
/// @fn WebrtcPlugin::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginInitialize()
///
/// @see FB::FactoryBase::globalPluginInitialize
///////////////////////////////////////////////////////////////////////////////
void WebrtcPlugin::StaticInitialize()
{
    // Place one-time initialization stuff here; As of FireBreath 1.4 this should only
    // be called once per process
}

///////////////////////////////////////////////////////////////////////////////
/// @fn WebrtcPlugin::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginDeinitialize()
///
/// @see FB::FactoryBase::globalPluginDeinitialize
///////////////////////////////////////////////////////////////////////////////
void WebrtcPlugin::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here. As of FireBreath 1.4 this should
    // always be called just before the plugin library is unloaded
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  WebrtcPlugin constructor.  Note that your API is not available
///         at this point, nor the window.  For best results wait to use
///         the JSAPI object until the onPluginReady method is called
///////////////////////////////////////////////////////////////////////////////
WebrtcPlugin::WebrtcPlugin()
{
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  WebrtcPlugin destructor.
///////////////////////////////////////////////////////////////////////////////
WebrtcPlugin::~WebrtcPlugin()
{
    // This is optional, but if you reset m_api (the shared_ptr to your JSAPI
    // root object) and tell the host to free the retained JSAPI objects then
    // unless you are holding another shared_ptr reference to your JSAPI object
    // they will be released here.
    releaseRootJSAPI();
    m_host->freeRetainedObjects();
}

void WebrtcPlugin::onPluginReady()
{
    // When this is called, the BrowserHost is attached, the JSAPI object is
    // created, and we are ready to interact with the page and such.  The
    // PluginWindow may or may not have already fire the AttachedEvent at
    // this point.
}

void WebrtcPlugin::shutdown()
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
/// boost::weak_ptr<WebrtcPlugin> for your JSAPI class.
///
/// Be very careful where you hold a shared_ptr to your plugin class from,
/// as it could prevent your plugin class from getting destroyed properly.
///////////////////////////////////////////////////////////////////////////////
FB::JSAPIPtr WebrtcPlugin::createJSAPI()
{
    // m_host is the BrowserHost
    return boost::make_shared<WebrtcPluginAPI>(FB::ptr_cast<WebrtcPlugin>(shared_from_this()), m_host);
}

bool WebrtcPlugin::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool WebrtcPlugin::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool WebrtcPlugin::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}
bool WebrtcPlugin::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *pWin)
{
    // The window is attached; act appropriately
    pThePluginWindow = pWin;
    return false;
}

bool WebrtcPlugin::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *pWin)
{
    // The window is about to be detached; act appropriately
    pThePluginWindow = NULL;
    return false;
}

bool WebrtcPlugin::onWindowResized(FB::ResizedEvent *evt, FB::PluginWindow *pWin)
{
    // The window has been resized; act appropriately
    pThePluginWindow = pWin;
    return false;
}

