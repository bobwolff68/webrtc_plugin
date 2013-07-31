#include "../GCPVideoRenderer.h"
#include "PluginWindowX11.h"
#include "libyuv.h"

namespace GoCast
{
    void ConvertToRGBA(uint8* pFrameBuffer, int width, int height)
    {
        const int stride = width*4;
        const int frameBufferSize = height*stride;
        uint8* pBufIter = pFrameBuffer;
        uint8* pBufEnd = pBufIter + frameBufferSize;

        while(pBufIter < pBufEnd)
        {
            pBufIter[3] = pBufIter[0];
            pBufIter[0] = pBufIter[2];
            pBufIter[2] = pBufIter[3];
            pBufIter[3] = 0xff;
            pBufIter += 4;
        }
    }
    
    gboolean RedrawWindow(gpointer data)
    {
        GCPVideoRenderer* pRenderer = reinterpret_cast<GCPVideoRenderer*>(data);
        return pRenderer->OnWindowRefresh();
    }

    bool GCPVideoRenderer::OnWindowRefresh(FB::RefreshEvent* pEvt)
    {
        static uint8* pScaleBuf = AllocBuffer(m_width*m_height*32);
        boost::mutex::scoped_lock winLock(m_winMutex);
        FB::PluginWindowX11* pWinX11 = reinterpret_cast<FB::PluginWindowX11*>(m_pWin);
        GtkWidget* pRenderArea = pWinX11->getWidget();
                
        libyuv::ARGBScale(m_pFrameBuffer, m_width*4, m_width, m_height,
                          pScaleBuf, m_pWin->getWindowWidth()*4,
                          m_pWin->getWindowWidth(), m_pWin->getWindowHeight(), libyuv::kFilterBilinear);
                          
        GoCast::ConvertToRGBA(pScaleBuf, m_pWin->getWindowWidth(), m_pWin->getWindowHeight());

        gdk_draw_rgb_32_image(
            pRenderArea->window,
            pRenderArea->style->fg_gc[GTK_STATE_NORMAL],
            0,
            0,
            m_pWin->getWindowWidth(),
            m_pWin->getWindowHeight(),
            GDK_RGB_DITHER_MAX,
            pScaleBuf,
            m_pWin->getWindowWidth()*4
        );

        return false;
    }
    
    void GCPVideoRenderer::ConvertToRGBA()
    {
    }
    
    void GCPVideoRenderer::InvalidateWindow()
    {
        g_idle_add(RedrawWindow, this);
    }
    
    uint8* GCPVideoRenderer::AllocBuffer(size_t size)
    {
        uint8* pBuf = NULL;
        
        if(0 < size)
        {
            posix_memalign(reinterpret_cast<void**>(&pBuf), 16, size);
        }
        
        return pBuf;
    }
    
    void GCPVideoRenderer::FreeBuffer(uint8 *pBuf)
    {
        if(NULL != pBuf)
        {
            free(pBuf);
        }
    }
}

