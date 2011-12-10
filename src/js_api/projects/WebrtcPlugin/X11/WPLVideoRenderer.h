#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_LINUX))

#ifndef WebrtcPlugin_WPLVideoRenderer_h
#define WebrtcPlugin_WPLVideoRenderer_h

#include <gtk/gtk.h>
#include <string>
#include "talk/session/phone/videorenderer.h"
#include "talk/session/phone/videoframe.h"
#include "talk/session/phone/videocommon.h"
#include "talk/base/scoped_ptr.h"

#define GOCAST_DEFAULT_RENDER_WIDTH     352
#define GOCAST_DEFAULT_RENDER_HEIGHT    288

namespace GoCast
{
	class VideoRenderer : public cricket::VideoRenderer
	{
	public:
		static VideoRenderer* Create(const std::string& peerName, const int width, const int height);
		static void Destroy(VideoRenderer* pRenderer);
		static gboolean OnRefreshRenderArea(gpointer pData);
	
	public:
		bool Init();
		void Deinit();
		void RedrawRenderArea();
		virtual bool SetSize(int width, int height, int reserved);
		virtual bool RenderFrame(const cricket::VideoFrame* pFrame);

	protected:
		explicit VideoRenderer(const std::string& peerName, const int width, int height);
		virtual ~VideoRenderer();

	protected:
		talk_base::scoped_array<uint8> m_spFrmBuf;
		GtkWidget* m_pWindow;
		GtkWidget* m_pRenderArea;

	protected:
		const std::string& m_peerName;
		const int m_width;
		const int m_height;
	};
}

#endif

#endif
