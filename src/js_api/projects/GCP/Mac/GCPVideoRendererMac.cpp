#include "../GCPVideoRenderer.h"

namespace GoCast
{
    
    bool GCPVideoRenderer::OnWindowRefresh(FB::RefreshEvent* pEvt)
    {
        FB::CoreGraphicsDraw* pCgDrawEvt(static_cast<FB::CoreGraphicsDraw*>(pEvt));
        CGContextRef pContext = pCgDrawEvt->context;
        boost::mutex::scoped_lock winLock(m_winMutex);

        const int stride = m_width*4;    
        const int frameBufferSize = m_height*stride;
        static SInt32 osMajorVersion = 0;
        static SInt32 osMinorVersion = 0;
        static CGInterpolationQuality interpolationMode = kCGInterpolationNone;
        
        if(0 == osMajorVersion || 0 == osMinorVersion)
        {
            if(noErr != Gestalt(gestaltSystemVersionMajor, &osMajorVersion))
            {
                osMajorVersion = 10;
            }
            if(noErr != Gestalt(gestaltSystemVersionMinor, &osMinorVersion))
            {
                osMinorVersion = 6;
            }
            if(10 <= osMajorVersion && 7 <= osMinorVersion)
            {
                interpolationMode = kCGInterpolationDefault;
            }
        }
        
        if(NULL == pContext || NULL == m_pFrameBuffer)
        {
            return false;
        }
        
        int winWidth = pCgDrawEvt->bounds.right - pCgDrawEvt->bounds.left;
        int winHeight = pCgDrawEvt->bounds.bottom - pCgDrawEvt->bounds.top;
        
        if(winWidth<=1 || winHeight<=1)
            return false;
        
        CGContextSaveGState(pContext);        
        CGContextSetShouldAntialias(pContext, true);
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGImageRef cgImage = CGImageCreate(m_width, m_height, 8, 32, stride, colorSpace, 
                                           kCGImageAlphaNoneSkipLast,
                                           CGDataProviderCreateWithData(NULL,
                                                                        m_pFrameBuffer,
                                                                        frameBufferSize,
                                                                        NULL),
                                           NULL, false, kCGRenderingIntentDefault);
        if(NULL == cgImage)
        {
            CGColorSpaceRelease(colorSpace);
            CGContextRestoreGState(pContext);
            return false;
        }
        
        CGContextSetInterpolationQuality(pContext, interpolationMode);
        CGContextTranslateCTM(pContext, 0, winHeight);
        CGContextScaleCTM(pContext, 1, -1);
        CGContextDrawImage(pContext, CGRectMake(0, 0, winWidth, winHeight), cgImage);
        
        CGImageRelease(cgImage);
        CGColorSpaceRelease(colorSpace);
        CGContextRestoreGState(pContext);
        
        return true;
    }
    
    void GCPVideoRenderer::ConvertToRGBA()
    {
        const int stride = m_width << 2;
        const int frameBufferSize = m_height*stride;
        uint8* pBufIter = m_pFrameBuffer;
        uint8* pBufEnd = pBufIter + frameBufferSize;

        while(pBufIter < pBufEnd)
        {
            SWAP_VALUES(pBufIter[0], pBufIter[2], pBufIter[3], 0xff);
            SWAP_VALUES(pBufIter[4], pBufIter[6], pBufIter[7], 0xff);
            SWAP_VALUES(pBufIter[8], pBufIter[10], pBufIter[11], 0xff);
            SWAP_VALUES(pBufIter[12], pBufIter[14], pBufIter[15], 0xff);
            pBufIter += 16;
        }
    }
    
    void GCPVideoRenderer::InvalidateWindow()
    {
        m_pWin->InvalidateWindow();
    }
    
    uint8* GCPVideoRenderer::AllocBuffer(size_t size)
    {
        uint8* pBuf = NULL;
        
        if(0 < size)
        {
            pBuf = new uint8[size];
        }
        
        return pBuf;
    }
    
    void GCPVideoRenderer::FreeBuffer(uint8 *pBuf)
    {
        if(NULL != pBuf)
        {
            delete pBuf;
        }
    }
}
