#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for GoCastPlayer
#
#\**********************************************************/

set(PLUGIN_NAME "GCP")
set(PLUGIN_PREFIX "GCP")
set(COMPANY_NAME "GoCast")

# ActiveX constants:
set(FBTYPELIB_NAME GCPLib)
set(FBTYPELIB_DESC "GCP 1.0 Type Library")
set(IFBControl_DESC "GCP Control Interface")
set(FBControl_DESC "GCP Control Class")
set(IFBComJavascriptObject_DESC "GCP IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "GCP ComJavascriptObject Class")
set(IFBComEventSource_DESC "GCP IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 84420c29-df35-5a1b-a654-09b764e87498)
set(IFBControl_GUID e451cda4-ff33-59e9-8e2e-89808d84fceb)
set(FBControl_GUID 0ab5647f-2036-55af-b282-4cfd5e569c7b)
set(IFBComJavascriptObject_GUID 9e2590c1-6b8a-5a45-a270-670e0ac49a9b)
set(FBComJavascriptObject_GUID c1e0bb4d-af26-5352-b526-51675fea2c58)
set(IFBComEventSource_GUID 7cdde218-af55-51ea-a4dd-9a2250b7a9c2)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "GoCast.GCP")
set(MOZILLA_PLUGINID "gocast.it/GCP")

# strings
set(FBSTRING_CompanyName "GoCast")
set(FBSTRING_PLUGIN_VERSION "2.2.0.0")
set(FBSTRING_FileDescription "GCP ${FBSTRING_PLUGIN_VERSION}")
set(FBSTRING_LegalCopyright "Copyright 2013 GoCast")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "GoCastPlayer")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "GoCastPlayer")
set(FBSTRING_MIMEType "application/x-gocastplayer")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

#set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 1)
set(FBMAC_USE_COCOA 1)
set(FBMAC_USE_COREGRAPHICS 1)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)

# Add logging library
add_firebreath_library(log4cplus)

# Add boost's filesystem library
add_boost_library(filesystem)
