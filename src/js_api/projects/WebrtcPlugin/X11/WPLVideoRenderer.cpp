#include <gdk/gdk.h>
#include <assert.h>
#include "WPLVideoRenderer.h"

namespace GoCast
{
    VideoRenderer* VideoRenderer::Create(const std::string& peerName,
                                         const int width,
                                         const int height)
    {
	    return new VideoRenderer(peerName, width, height);
    }

    void VideoRenderer::Destroy(VideoRenderer* pRenderer)
    {
	    delete pRenderer;
    }

    gboolean VideoRenderer::OnRefreshRenderArea(gpointer pData)
    {
	    VideoRenderer* pRenderer = reinterpret_cast<VideoRenderer*>(pData);
	    pRenderer->RedrawRenderArea();
	    return FALSE;
    }

    bool VideoRenderer::Init()
    {
	    if(NULL!=m_pWindow || NULL!=m_pRenderArea)
	    {
		    return false;
	    }

	    m_pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	    if(NULL == m_pWindow)
	    {
		    return false;
	    }

	    gtk_window_set_position(GTK_WINDOW(m_pWindow), GTK_WIN_POS_CENTER);
	    gtk_window_set_default_size(GTK_WINDOW(m_pWindow), m_width, m_height);
	    gtk_window_set_resizable(GTK_WINDOW(m_pWindow), FALSE);
	    gtk_window_set_deletable(GTK_WINDOW(m_pWindow), FALSE);
	    gtk_window_set_title(GTK_WINDOW(m_pWindow), m_peerName.c_str());
	    gtk_container_set_border_width(GTK_CONTAINER(m_pWindow), 0);

	    m_pRenderArea = gtk_drawing_area_new();
	    if(NULL == m_pRenderArea)
	    {
		    return false;
	    }

	    gtk_widget_set_size_request(m_pRenderArea, m_width, m_height);
	    gtk_container_add(GTK_CONTAINER(m_pWindow), m_pRenderArea);
	    gtk_widget_show_all(m_pWindow);
	
	    return true;
    }

    void VideoRenderer::Deinit()
    {
	    gtk_widget_destroy(m_pRenderArea);
	    gtk_widget_destroy(m_pWindow);
    }

    void VideoRenderer::RedrawRenderArea()
    {
	    gdk_threads_enter();

	    gdk_draw_rgb_32_image(
	      m_pRenderArea->window,
	      m_pRenderArea->style->fg_gc[GTK_STATE_NORMAL],
	      0,
	      0,
	      m_width,
	      m_height,
	      GDK_RGB_DITHER_MAX,
	      m_spFrmBuf.get(),
	      m_width*4
	    );

	    gdk_threads_leave();
    }

    bool VideoRenderer::SetSize(int width, int height, int reserved)
    {
	    return true;
    }

    bool VideoRenderer::RenderFrame(const cricket::VideoFrame* pFrame)
    {
	    int frmBufSize = m_width*m_height*4;
	
	    gdk_threads_enter();

	    pFrame->ConvertToRgbBuffer(
	      cricket::FOURCC_ARGB,
	      m_spFrmBuf.get(),
	      frmBufSize,
	      m_width*4
	    );

	    uint8* pBufIter = m_spFrmBuf.get();
	    uint8* pBufEnd = pBufIter + frmBufSize;

	    while(pBufIter < pBufEnd)
	    {
		    pBufIter[3] = pBufIter[0];
		    pBufIter[0] = pBufIter[2];
		    pBufIter[2] = pBufIter[3];
		    pBufIter[3] = 0xff;
		    pBufIter += 4;
	    }

	    gdk_threads_leave();
	
	    g_idle_add(VideoRenderer::OnRefreshRenderArea, this);
	
	    return true;
    }

    VideoRenderer::VideoRenderer(const std::string& peerName,
                                 const int width,
                                 const int height):
    m_pWindow(NULL),
    m_pRenderArea(NULL),
    m_peerName(peerName),
    m_width(width),
    m_height(height)
    {
	    m_spFrmBuf.reset(new uint8[m_width*m_height*4]);
    }

    VideoRenderer::~VideoRenderer()
    {
	    m_spFrmBuf.reset(NULL);
    }
}
