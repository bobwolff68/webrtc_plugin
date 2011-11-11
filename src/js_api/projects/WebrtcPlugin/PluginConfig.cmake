#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for WebrtcPlugin
#
#\**********************************************************/

set(PLUGIN_NAME "WebrtcPlugin")
set(PLUGIN_PREFIX "WPL")
set(COMPANY_NAME "XVDTHUSA")

# ActiveX constants:
set(FBTYPELIB_NAME WebrtcPluginLib)
set(FBTYPELIB_DESC "WebrtcPlugin 1.0 Type Library")
set(IFBControl_DESC "WebrtcPlugin Control Interface")
set(FBControl_DESC "WebrtcPlugin Control Class")
set(IFBComJavascriptObject_DESC "WebrtcPlugin IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "WebrtcPlugin ComJavascriptObject Class")
set(IFBComEventSource_DESC "WebrtcPlugin IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID d01735a7-d7ff-5504-a5a6-957d3ffe171c)
set(IFBControl_GUID 23148c5a-a0e3-5a1d-b365-29a8945cffeb)
set(FBControl_GUID c30e8e5b-0b26-5267-9b93-1e41873c0db4)
set(IFBComJavascriptObject_GUID accd017e-2bf6-5533-ae64-173400171b44)
set(FBComJavascriptObject_GUID fbbf16fe-3d02-5d77-b218-206bec8c697d)
set(IFBComEventSource_GUID 38350cb3-37de-5a8d-8496-ede359fed39a)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "XVDTHUSA.WebrtcPlugin")
set(MOZILLA_PLUGINID "xvdth.com/WebrtcPlugin")

# strings
set(FBSTRING_CompanyName "XVDTH, USA")
set(FBSTRING_FileDescription "WebRTC Plugin")
set(FBSTRING_PLUGIN_VERSION "1.0.0.0")
set(FBSTRING_LegalCopyright "Copyright 2011 XVDTH, USA")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "WebrtcPlugin")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "WebrtcPlugin")
set(FBSTRING_MIMEType "application/x-webrtcplugin")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 0)
set(FBMAC_USE_COCOA 0)
set(FBMAC_USE_COREGRAPHICS 0)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)
