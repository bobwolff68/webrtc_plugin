#include "../GCPVideoRenderer.h"
#include "PluginWindowlessWin.h"

namespace GoCast
{

    bool GCPVideoRenderer::OnWindowRefresh(FB::RefreshEvent* pEvt)
    {
		boost::mutex::scoped_lock winLock(m_winMutex);
		int width = m_pWin->getWindowWidth();
		int height = m_pWin->getWindowHeight();
		HDC hdc;

		BITMAPINFO bitmapInfo;
		bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo.bmiHeader.biBitCount = 32;
		bitmapInfo.bmiHeader.biCompression = BI_RGB;
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biSizeImage = m_width*m_height*4;
		bitmapInfo.bmiHeader.biWidth = m_width;
		bitmapInfo.bmiHeader.biHeight = m_height;

		FB::PluginWindowlessWin* pWinlessWindowsWin = dynamic_cast<FB::PluginWindowlessWin*>(m_pWin);
		if(NULL != pWinlessWindowsWin && NULL != m_pFrameBuffer)
		{
			FB::Rect winRect = m_pWin->getWindowPosition();
			hdc = pWinlessWindowsWin->getHDC();
			SetStretchBltMode(hdc, HALFTONE);
			StretchDIBits(hdc, winRect.left, winRect.bottom-1, width, -height, 0, 0, m_width, m_height, m_pFrameBuffer, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		}

        return true;
    }
    
    void GCPVideoRenderer::ConvertToRGBA()
    {
        // Not needed so empty
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
			pBuf = (uint8*) _aligned_malloc(size, 16);
		}

		return pBuf;
	}

	void GCPVideoRenderer::FreeBuffer(uint8* pBuf)
	{
		//Assume pBuf as aligned
		if(NULL != pBuf)
		{
			_aligned_free(pBuf);
		}
	}
}
