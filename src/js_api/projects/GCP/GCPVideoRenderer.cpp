#include "GCPVideoRenderer.h"
#include "GCPMediaStream.h"
#include "libyuv.h"

namespace GoCast
{
    GCPVideoRenderer::GCPVideoRenderer(FB::PluginWindow* pWin)
    : m_pFrameBuffer(NULL)
    , m_pMirrorBuffer(NULL)
    , m_pWin(pWin)
    , m_width(0)
    , m_height(0)
    , m_bPreview(false)
    , m_bRenderLogged(false)
    {

    }
    
    GCPVideoRenderer::~GCPVideoRenderer()
    {
		FreeBuffer(m_pFrameBuffer);
		FreeBuffer(m_pMirrorBuffer);
    }
    
    void GCPVideoRenderer::SetSize(int width, int height)
    {
        boost::mutex::scoped_lock winLock(m_winMutex);
        m_width = width;
        m_height = height;
    }
    
    void GCPVideoRenderer::RenderFrame(const cricket::VideoFrame* pFrame)
    {
        boost::mutex::scoped_lock winLock(m_winMutex);
        
        if(NULL == m_pFrameBuffer)
        {
            m_width = pFrame->GetWidth();
            m_height = pFrame->GetHeight();
            m_pFrameBuffer = AllocBuffer(m_width*m_height*4);
            m_pMirrorBuffer = AllocBuffer(m_width*m_height*4);
        }

        int stride = m_width*4;
        const int frameBufferSize = m_height*stride;
        pFrame->ConvertToRgbBuffer(cricket::FOURCC_ARGB,
                                   m_pMirrorBuffer,
                                   frameBufferSize,
                                   stride);
        
        if(false == MirrorIfPreview(stride))
        {
            return;
        }
        
        //convert to rgba and correct alpha
        ConvertToRGBA();
        
        if(false == m_bRenderLogged)
        {
            FBLOG_INFO_CUSTOM("GCPVideoRenderer::RenderFrame", "First frame rendered");
            m_bRenderLogged = true;
        }
        
        //trigger window refresh event
        InvalidateWindow();        
    }

    bool GCPVideoRenderer::MirrorIfPreview(int stride)
    {
        
		if(0 >= m_width || 0 >= m_height)
		{
			return false;
		}
        if(true == m_bPreview)
        {
            if(0 > libyuv::ARGBMirror(m_pMirrorBuffer, stride, m_pFrameBuffer, stride, m_width, m_height))
            {
                return false;
            }
        }
        else
        {
            SWAP_BUFFER_POINTERS(m_pMirrorBuffer, m_pFrameBuffer);
        }
        
        return true;
    }
}
