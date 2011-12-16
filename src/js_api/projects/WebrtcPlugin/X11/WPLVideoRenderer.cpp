#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_LINUX))

#include <gdk/gdk.h>
#include <assert.h>
#include "WPLVideoRenderer.h"
#include "rtc_common.h"

#ifdef GOCAST_PLUGIN
#include "X11/PluginWindowX11.h"
extern FB::PluginWindow* pThePluginWindow;
GoCast::VideoRenderer* GoCast::VideoRenderer::s_pHead = NULL;
#endif

int GoCast::VideoRenderer::s_numRenderers = 0;

namespace GoCast
{
    VideoRenderer* VideoRenderer::Create(const std::string& peerName,
                                         const int width,
                                         const int height,
                                         ThreadSafeMessageQueue* pEvtQ)
    {
	    return new VideoRenderer(peerName, width, height, pEvtQ);
    }

    void VideoRenderer::Destroy(VideoRenderer* pRenderer)
    {
	    delete pRenderer;
    }

    gboolean VideoRenderer::OnRefreshRenderArea(gpointer pData)
    {
	    VideoRenderer* pRenderer = reinterpret_cast<VideoRenderer*>(pData);	    
	    if(0 < s_numRenderers)
	    {
	        pRenderer->RedrawRenderArea();
	    }
	    
	    return FALSE;
    }

    bool VideoRenderer::Init()
    {

#ifndef GOCAST_PLUGIN
	    if(NULL!=m_pWindow || NULL!=m_pRenderArea)
	    {
		    return false;
	    }

	    m_pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	    if(NULL == m_pWindow)
	    {
		    return false;
	    }

	    gtk_window_set_position(GTK_WINDOW(m_pWindow), GTK_WIN_POS_NONE);
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
#else
        FB::PluginWindowX11* pThePluginWindowX11 = 
            reinterpret_cast<FB::PluginWindowX11*>(pThePluginWindow);
        m_pRenderArea = pThePluginWindowX11->getWidget();
        
        if(NULL != s_pHead)
        {
            s_pHead->SetPrev(this);
        }
        
        m_rendererIndex = s_numRenderers;
        s_pHead = this;     
#endif
        
        s_numRenderers++;
        
	    return true;
    }

    void VideoRenderer::Deinit()
    {
    
#ifndef GOCAST_PLUGIN
	    gtk_widget_destroy(m_pRenderArea);
	    gtk_widget_destroy(m_pWindow);
#else   
        if(NULL != m_pPrev)
        {
            m_pPrev->SetNext(m_pNext);
        }
        else
        {
            s_pHead = m_pNext;
        }
        
        if(NULL != m_pNext)
        {
            m_pNext->SetPrev(m_pPrev);
        }
#endif

        s_numRenderers--;
    }

    void VideoRenderer::RedrawRenderArea()
    {
	    gdk_threads_enter();

	    gdk_draw_rgb_32_image(
	      m_pRenderArea->window,
	      m_pRenderArea->style->fg_gc[GTK_STATE_NORMAL],

#ifdef GOCAST_PLUGIN
	      (m_rendererIndex=(m_pNext?(m_pNext->RendererIndex()+1):m_rendererIndex))*m_width,
#else
          0,
#endif

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
                                 const int height,
                                 ThreadSafeMessageQueue* pEvtQ):
    m_pWindow(NULL),
    m_pRenderArea(NULL),
    m_peerName(peerName),
    m_width(width),
    m_height(height)
    
#ifdef GOCAST_PLUGIN
    , m_rendererIndex(0)
    , m_pNext(s_pHead)
    , m_pPrev(NULL)
#endif
    
    , m_pEvtQ(pEvtQ)
    {
	    m_spFrmBuf.reset(new uint8[m_width*m_height*4]);
	    
	    if(NULL != m_pEvtQ)
	    {
	        ThreadSafeMessageQueue::ParsedMessage event;
	        event["type"] = "RendererAdd";
	        event["message"] = ToString(m_width);
	        event["message"] += ":";
	        event["message"] += ToString(m_height);
	        m_pEvtQ->PostMessage(event);
	        
	        //TODO: Hack - wait till resize happens from javascript
	        usleep(100000);
	    }
    }

    VideoRenderer::~VideoRenderer()
    {
	    m_spFrmBuf.reset(NULL);

	    if(NULL != m_pEvtQ)
	    {
	        ThreadSafeMessageQueue::ParsedMessage event;
	        event["type"] = "RendererRemove";
	        event["message"] = ToString(m_width);
	        event["message"] += ":";
	        event["message"] += ToString(m_height);
	        m_pEvtQ->PostMessage(event);

	        //TODO: Hack - wait till resize happens from javascript
	        usleep(100000);
	    }
    }
}

#endif
