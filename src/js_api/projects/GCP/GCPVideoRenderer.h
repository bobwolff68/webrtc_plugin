#ifndef Firebreath_GCPVideoRenderer_h
#define Firebreath_GCPVideoRenderer_h

//-------------- WebRTC headers ----------------------
#include "talk/app/webrtc/mediastreaminterface.h"
#include "talk/media/base/videoframe.h"
#include "talk/media/base/videocommon.h"
#include "talk/base/scoped_ptr.h"

//-------------- Firebreath Headers -------------------
#include <boost/thread/mutex.hpp>
#include "PluginEvents/DrawingEvents.h"
#include "PluginWindow.h"

#define SWAP_VALUES(x, y, t, tval) {\
    t = x;\
    x = y;\
    y = t;\
    t = tval;\
}

#define SWAP_BUFFER_POINTERS(x, y) {\
    uint8* t = x;\
    x = y;\
    y = t;\
}

namespace GoCast
{
    class GCPVideoRenderer: public webrtc::VideoRendererInterface
    {
    public:
        explicit GCPVideoRenderer(FB::PluginWindow* pWin);
        virtual ~GCPVideoRenderer();
        
        // cricket::VideoFrame overrides
        virtual void SetSize(int width, int height);
        virtual void RenderFrame(const cricket::VideoFrame* pFrame);
        
        // Refresh event handler
        virtual bool OnWindowRefresh(FB::RefreshEvent* pEvt = NULL);
        
    public:
        void SetPreviewMode(bool bPreview) { m_bPreview = bPreview; }

    protected:
        bool MirrorIfPreview(int stride);
        void ConvertToRGBA();
        void InvalidateWindow();
		uint8* AllocBuffer(size_t size);
		void FreeBuffer(uint8* pBuf);
        
    protected:
        uint8* m_pFrameBuffer;
        uint8* m_pMirrorBuffer;
        FB::PluginWindow* m_pWin;
        boost::mutex m_winMutex;
        int m_width;
        int m_height;
        bool m_bPreview;
        bool m_bRenderLogged;
    };
}

#endif

