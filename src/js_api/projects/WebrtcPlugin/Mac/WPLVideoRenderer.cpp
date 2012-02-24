#if(defined(GOCAST_ENABLE_VIDEO) && defined(GOCAST_MAC))

#include <assert.h>
#include <unistd.h>
#include "WPLVideoRenderer.h"
#include "rtc_common.h"
#include "Mac/PluginWindowMac.h"

extern pthread_mutex_t pluginWinMutex;
extern FB::PluginWindow* pThePluginWindow;
GoCast::VideoRenderer* GoCast::VideoRenderer::s_pHead = NULL;
int GoCast::VideoRenderer::s_numRenderers = 0;
std::deque<GoCast::VideoRenderer*> GoCast::VideoRenderer::s_refreshQueue;

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

    bool VideoRenderer::OnRefreshRenderArea(FB::RefreshEvent* pEvt,
                                            FB::PluginWindow* pWin)
    {        
	    if(0 < s_numRenderers)
	    {
            while(false == s_refreshQueue.empty())
            {
                VideoRenderer* pRenderer = s_refreshQueue.front();
                s_refreshQueue.pop_front();
                pRenderer->RedrawRenderArea(pEvt, pWin);
            }
	    }
	    
	    return false;
    }

    bool VideoRenderer::Init()
    {
        pthread_mutex_lock(&pluginWinMutex);
        
        if(NULL != s_pHead)
        {
            s_pHead->SetPrev(this);
        }
        
        m_rendererIndex = s_numRenderers;
        s_pHead = this;     
        s_numRenderers++;
        
        pthread_mutex_unlock(&pluginWinMutex);
        
	    return true;
    }

    void VideoRenderer::Deinit()
    {
        pthread_mutex_lock(&pluginWinMutex);
        
        s_refreshQueue.clear();
        
        if(NULL != m_pPrev)
        {
            m_pPrev->SetNext(m_pNext);
            m_pPrev->DecRendererIndex();
        }
        else
        {
            s_pHead = m_pNext;
        }
        
        if(NULL != m_pNext)
        {
            m_pNext->SetPrev(m_pPrev);
        }

        s_numRenderers--;
        
        pthread_mutex_unlock(&pluginWinMutex);
    }

    void VideoRenderer::RedrawRenderArea(FB::RefreshEvent* pEvt,
                                         FB::PluginWindow* pWin)
    {
        FB::CoreGraphicsDraw* pCgDrawEvt(static_cast<FB::CoreGraphicsDraw*>(pEvt));
        CGContextRef pContext = pCgDrawEvt->context;
        CGContextSetShouldAntialias(pContext, false);
        
        if(NULL == pContext)
        {
            return;
        }
        
        int winWidth = pCgDrawEvt->bounds.right - pCgDrawEvt->bounds.left;
        int winHeight = pCgDrawEvt->bounds.bottom - pCgDrawEvt->bounds.top;
        
        if(winWidth<=1 || winHeight<=1)
            return;
        
        CGContextSaveGState(pContext);
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGImageRef cgImage = CGImageCreate(m_width, m_height, 8, 32, 4*m_width, colorSpace, 
                                           kCGImageAlphaNoneSkipLast,
                                           CGDataProviderCreateWithData(NULL,
                                                                        m_spFrmBuf.get(),
                                                                        m_width*m_height*4,
                                                                        NULL),
                                           NULL, false, kCGRenderingIntentDefault);
        if(NULL == cgImage)
        {
            CGColorSpaceRelease(colorSpace);
            CGContextRestoreGState(pContext);
            return;
        }
        
        CGContextSetInterpolationQuality(pContext, kCGInterpolationNone);
        CGContextTranslateCTM(pContext, 0, winHeight);
        CGContextScaleCTM(pContext, 1, -1);
        CGContextDrawImage(pContext, CGRectMake(m_rendererIndex*m_width, 0, m_width, m_height), cgImage);
        
        CGImageRelease(cgImage);
        CGColorSpaceRelease(colorSpace);
        CGContextRestoreGState(pContext);        
    }

    bool VideoRenderer::SetSize(int width, int height, int reserved)
    {
	    return true;
    }

    bool VideoRenderer::RenderFrame(const cricket::VideoFrame* pFrame)
    {
	    int frmBufSize = m_width*m_height*4;
        
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
        
        pthread_mutex_lock(&pluginWinMutex);
        if(NULL != pThePluginWindow)
        {
            s_refreshQueue.push_back(this);
            
            if(0 == m_rendererIndex)
            {
                pThePluginWindow->InvalidateWindow();
            }
        }
        pthread_mutex_unlock(&pluginWinMutex);
	        
	    return true;
    }

    VideoRenderer::VideoRenderer(const std::string& peerName,
                                 const int width,
                                 const int height,
                                 ThreadSafeMessageQueue* pEvtQ)
    : m_peerName(peerName)
    , m_width(width)
    , m_height(height)    
    , m_rendererIndex(0)
    , m_pNext(s_pHead)
    , m_pPrev(NULL)
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
	    }
    }
}

#endif

