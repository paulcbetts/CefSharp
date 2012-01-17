#include "stdafx.h"

#include "CefWpfWebBrowser.h"

namespace CefSharp
{
    bool WpfClientAdapter::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
    {
        return false;
    }

    bool WpfClientAdapter::GetScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
    {
        return false;
    }

    bool WpfClientAdapter::GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY)
    {
        return false;
    }

    void WpfClientAdapter::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer)
    {
        int width, height;
        browser->GetSize(type, width, height);
		CefRect dummy;

		// NB: We don't actually use dirtyRect, we rerender the entire buffer
		// in the SetBuffer call regardless
        _wpfBrowserControl->SetBuffer(width, height, dummy, buffer);
    }

    void WpfClientAdapter::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor)
    {
        _wpfBrowserControl->SetCursor(cursor);
    }

	void WpfClientAdapter::OnContentsSizeChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int width, int height)
	{
		_wpfBrowserControl->UpdateContentSize((double)width, (double)height);
	}
}