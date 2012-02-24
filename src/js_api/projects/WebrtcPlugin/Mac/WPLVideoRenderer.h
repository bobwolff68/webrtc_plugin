#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_MAC))

#ifndef WebrtcPlugin_WPLVideoRenderer_h
#define WebrtcPlugin_WPLVideoRenderer_h

#include <string>
#include <deque>
#include "../WPLThreadSafeMessageQueue.h"
#include "talk/session/phone/videorenderer.h"
#include "talk/session/phone/videoframe.h"
#include "talk/session/phone/videocommon.h"
#include "talk/base/scoped_ptr.h"

#include "PluginEvents/DrawingEvents.h"
#include "PluginWindow.h"

#define GOCAST_DEFAULT_RENDER_WIDTH     352
#define GOCAST_DEFAULT_RENDER_HEIGHT    288

namespace GoCast
{
	class VideoRenderer : public cricket::VideoRenderer
	{
	public:	
		static VideoRenderer* Create(const std::string& peerName, 
		                             const int width, 
		                             const int height,
		                             ThreadSafeMessageQueue* pEvtQ);

		static void Destroy(VideoRenderer* pRenderer);
		static bool OnRefreshRenderArea(FB::RefreshEvent* pEvt,
                                        FB::PluginWindow* pWin);
	
	public:
		bool Init();
		void Deinit();
		void RedrawRenderArea(FB::RefreshEvent* pEvt,
                              FB::PluginWindow* pWin);
		virtual bool SetSize(int width, int height, int reserved);
		virtual bool RenderFrame(const cricket::VideoFrame* pFrame);

	protected:	
		explicit VideoRenderer(const std::string& peerName, 
		                       const int width, 
		                       const int height,
		                       ThreadSafeMessageQueue* pEvtQ);
		virtual ~VideoRenderer();

	protected:
		const std::string& m_peerName;
		const int m_width;
		const int m_height;
		
    public:
        VideoRenderer* Prev() const { return m_pPrev; }
        void SetPrev(VideoRenderer* pRenderer) { m_pPrev = pRenderer; }
        VideoRenderer* Next() const { return m_pNext; }
        void SetNext(VideoRenderer* pRenderer) { m_pNext = pRenderer; }
        int RendererIndex() const { return m_rendererIndex; }
        void DecRendererIndex() { m_rendererIndex--; }
            
    protected:
        int m_rendererIndex;
        VideoRenderer* m_pNext;
        VideoRenderer* m_pPrev;
        ThreadSafeMessageQueue* m_pEvtQ;
        talk_base::scoped_array<uint8> m_spFrmBuf;        
    
    protected:
        static VideoRenderer* s_pHead;
        static int s_numRenderers;      
        static std::deque<VideoRenderer*> s_refreshQueue;
	};
}

#endif

#endif

