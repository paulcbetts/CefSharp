#include "stdafx.h"

#include "CefWpfWebBrowser.h"
#include "JsTask.h"
#include "ScriptException.h"

namespace CefSharp
{
    void CefWpfWebBrowser::Load(String^ url)
    {
        if (!WaitForInitialized()) return;

        _loadCompleted->Reset();
        _clientAdapter->GetCefBrowser()->GetMainFrame()->LoadURL(toNative(url));
    }

    void CefWpfWebBrowser::Stop()
    {
        if (!WaitForInitialized()) return;

        _clientAdapter->GetCefBrowser()->StopLoad();
    }

    void CefWpfWebBrowser::Back()
    {
        if (!WaitForInitialized()) return;

        _clientAdapter->GetCefBrowser()->GoBack();
    }

    void CefWpfWebBrowser::Forward()
    {
        if (!WaitForInitialized()) return;

        _clientAdapter->GetCefBrowser()->GoForward();
    }

    void CefWpfWebBrowser::Reload()
    {
        Reload(false);
    }

    void CefWpfWebBrowser::Reload(bool ignoreCache)
    {
        if (!WaitForInitialized()) return;

        if(ignoreCache)
        {
            _clientAdapter->GetCefBrowser()->ReloadIgnoreCache();
        }
        else
        {
            _clientAdapter->GetCefBrowser()->Reload();
        }
    }

    void CefWpfWebBrowser::Print()
    {
        if (!WaitForInitialized()) return;

        _clientAdapter->GetCefBrowser()->GetMainFrame()->Print();
    }

    String^ CefWpfWebBrowser::RunScript(String^ script, String^ scriptUrl, int startLine)
    {
        if (!WaitForInitialized()) return nullptr;

        return RunScript(script, scriptUrl, startLine, -1);
    }

    String^ CefWpfWebBrowser::RunScript(String^ script, String^ scriptUrl, int startLine, int timeout)
    {
        if (!WaitForInitialized()) return nullptr;
        
        _jsError = false;
        _jsResult = nullptr;
        /*
        script = 
            "(function() {"
            "   try { "
            "      __js_run_done(" + script + ");"
            "   } catch(e) {"
            "      __js_run_err(e);"
            "   }"
            "})();";
        */
        
        /*
        CefRefPtr<JsTask> task = new JsTask(this, toNative(script), toNative(scriptUrl), startLine);
        _clientAdapter->GetCefBrowser()->GetMainFrame()->ExecuteJavaScriptTask(static_cast<CefRefPtr<CefV8Task>>(task));

        if(!_runJsFinished->WaitOne(timeout))
        {
            throw gcnew TimeoutException(L"Timed out waiting for JavaScript to return");
        }
        */

        _clientAdapter->GetCefBrowser()->GetMainFrame()->ExecuteJavaScript(toNative(script), toNative(scriptUrl), startLine);

        if(_jsError == false) 
        {
            return _jsResult;
        }
        throw gcnew ScriptException("An error occurred during javascript execution");
    }

    void CefWpfWebBrowser::OnInitialized()
    {
        _browserInitialized->Set();
    }

    void CefWpfWebBrowser::SetTitle(String^ title)
    {
        _title = title;
        PropertyChanged(this, gcnew PropertyChangedEventArgs(L"Title"));
    }

    void CefWpfWebBrowser::SetToolTip(String^ text)
    {

    }

    void CefWpfWebBrowser::SetAddress(String^ address)
    {
        _address = address;
        PropertyChanged(this, gcnew PropertyChangedEventArgs(L"Address"));
    }

    void CefWpfWebBrowser::SetNavState(bool isLoading, bool canGoBack, bool canGoForward)
    {
        if(isLoading != _isLoading) 
        {
            _isLoading = isLoading;
            PropertyChanged(this, gcnew PropertyChangedEventArgs(L"IsLoading"));
        }

        if(canGoBack != _canGoBack) 
        {
            _canGoBack = canGoBack;
            PropertyChanged(this, gcnew PropertyChangedEventArgs(L"CanGoBack"));
        }

        if(canGoForward != _canGoForward)
        {
            _canGoForward = canGoForward;
            PropertyChanged(this, gcnew PropertyChangedEventArgs(L"CanGoForward"));
        }
    }

    void CefWpfWebBrowser::AddFrame(CefRefPtr<CefFrame> frame)
    {
        _loadCompleted->AddCount();
    }

    void CefWpfWebBrowser::FrameLoadComplete(CefRefPtr<CefFrame> frame)
    {
        EventArgs^ ea = gcnew EventArgs();
        LoadFinished(this, ea);
    }

    void CefWpfWebBrowser::SetJsResult(String^ result)
    {
        _jsResult = result;
        _runJsFinished->Set();
    }

    void CefWpfWebBrowser::SetJsError()
    {
        _jsError = true;
        _runJsFinished->Set();
    }

    void CefWpfWebBrowser::RaiseConsoleMessage(String^ message, String^ source, int line)
    {
        ConsoleMessage(this, gcnew ConsoleMessageEventArgs(message, source, line));
    }

    void CefWpfWebBrowser::UpdateContentSize(double width, double height)
    {
        _contentWidth = width;
        _contentHeight = height;

        PropertyChanged(this, gcnew PropertyChangedEventArgs(L"ContentWidth"));
        PropertyChanged(this, gcnew PropertyChangedEventArgs(L"ContentHeight"));
    }

    bool CefWpfWebBrowser::WaitForInitialized()
    {
        //if (IsInitialized) return;

        if (System::ComponentModel::DesignerProperties::GetIsInDesignMode(this)) 
        {
            return true;
        }

        if (_loadCompleted == nullptr || _address == nullptr)
        {
            Visual^ parent = (Visual^)VisualTreeHelper::GetParent(this);
            if (parent == nullptr) 
            {
                return false;
            }

            HwndSource^ source = (HwndSource^)PresentationSource::FromVisual(parent);
            Setup(source, "about:blank");
        }

        return _clientAdapter->GetCefBrowser() != NULL;
    }

    void CefWpfWebBrowser::OnApplyTemplate()
    {
        ContentControl::OnApplyTemplate();

        if (System::ComponentModel::DesignerProperties::GetIsInDesignMode(this)) 
        {
            TextBlock^ textBlock = gcnew TextBlock();
            textBlock->Text = L"This is a Chrome Control";
            textBlock->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
            textBlock->VerticalAlignment = System::Windows::VerticalAlignment::Center;

            Content = textBlock;
            return;
        }

        WaitForInitialized();

        _image = (Image^)GetTemplateChild("PART_Image");

        if (_image == nullptr)
        {
            Content = _image = gcnew Image();

            _image->Stretch = Stretch::None;
            _image->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;
            _image->VerticalAlignment = System::Windows::VerticalAlignment::Top;
        }
    }

    Size CefWpfWebBrowser::ArrangeOverride(Size size)
    {
		if (!System::ComponentModel::DesignerProperties::GetIsInDesignMode(this) && WaitForInitialized())
		{
			PresentationSource^ source = PresentationSource::FromVisual(this);
			Matrix^ deviceTransform = source->CompositionTarget->TransformToDevice;

			Point^ deviceSize = deviceTransform->Transform(Point(size.Width, size.Height));

			_clientAdapter->GetCefBrowser()->SetSize(PET_VIEW, (int)deviceSize->X, (int)deviceSize->Y);
		}

        return ContentControl::ArrangeOverride(size);
    }

    void CefWpfWebBrowser::OnGotFocus(RoutedEventArgs^ e)
    {
        if (!System::ComponentModel::DesignerProperties::GetIsInDesignMode(this)) 
        {
            _clientAdapter->GetCefBrowser()->SendFocusEvent(true);
        }

        ContentControl::OnGotFocus(e);
    }

    void CefWpfWebBrowser::OnLostFocus(RoutedEventArgs^ e)
    {
        if (!System::ComponentModel::DesignerProperties::GetIsInDesignMode(this)) 
        {
            _clientAdapter->GetCefBrowser()->SendFocusEvent(false);
        }

        ContentControl::OnLostFocus(e);
    }

    void CefWpfWebBrowser::OnMouseMove(MouseEventArgs^ e)
    {
        Point point = e->GetPosition(this);
        _clientAdapter->GetCefBrowser()->SendMouseMoveEvent((int)point.X, (int)point.Y, false);
    }

    void CefWpfWebBrowser::OnMouseWheel(MouseWheelEventArgs^ e)
    {
        Point point = e->GetPosition(this);
        _clientAdapter->GetCefBrowser()->SendMouseWheelEvent((int)point.X, (int)point.Y, e->Delta);
    }

    void CefWpfWebBrowser::OnMouseLeave(MouseEventArgs^ e)
    {
        _clientAdapter->GetCefBrowser()->SendMouseMoveEvent(0, 0, true);
    }

    void CefWpfWebBrowser::OnMouseDown(MouseButtonEventArgs^ e)
    {
        Keyboard::Focus(this); // XXX: temporary

        Point point = e->GetPosition(this);
        CefBrowser::MouseButtonType mbt;
        if (e->RightButton == MouseButtonState::Pressed)
        {
            mbt = CefBrowser::MouseButtonType::MBT_RIGHT;
        }
        else if (e->LeftButton == MouseButtonState::Pressed)
        {
            mbt = CefBrowser::MouseButtonType::MBT_LEFT;
        }

        _clientAdapter->GetCefBrowser()->SendMouseClickEvent((int)point.X, (int)point.Y, mbt, false, 1);
    }

    void CefWpfWebBrowser::OnMouseUp(MouseButtonEventArgs^ e)
    {
        Point point = e->GetPosition(this);
        CefBrowser::MouseButtonType mbt;
        if (e->RightButton == MouseButtonState::Pressed)
        {
            mbt = CefBrowser::MouseButtonType::MBT_RIGHT;
        }
        else if (e->LeftButton == MouseButtonState::Pressed)
        {
            mbt = CefBrowser::MouseButtonType::MBT_LEFT;
        }

        _clientAdapter->GetCefBrowser()->SendMouseClickEvent((int)point.X, (int)point.Y, mbt, true, 1);
    }

    void CefWpfWebBrowser::SetCursor(CefCursorHandle cursor)
    {
        SafeFileHandle^ handle = gcnew SafeFileHandle((IntPtr)cursor, false);
        Dispatcher->BeginInvoke(DispatcherPriority::Render,
            gcnew Action<SafeFileHandle^>(this, &CefWpfWebBrowser::SetCursor), handle);
    }

    void CefWpfWebBrowser::SetCursor(SafeFileHandle^ handle)
    {
        Cursor = CursorInteropHelper::Create(handle);
    }

    IntPtr CefWpfWebBrowser::SourceHook(IntPtr hWnd, int message, IntPtr wParam, IntPtr lParam, bool% handled)
    {
        handled = false;

        if (System::ComponentModel::DesignerProperties::GetIsInDesignMode(this)) 
        {
            return IntPtr::Zero;
        }

        switch(message)
        {
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_CHAR:
            case WM_SYSCHAR:
            case WM_IME_CHAR:
                if (!IsFocused)
                {
                    break;
                }

                CefBrowser::KeyType type = KT_CHAR;
                bool sysChar = false, imeChar = false;

                if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
                {
                    type = KT_KEYDOWN;
                }
                else if (message == WM_KEYUP || message == WM_SYSKEYUP)
                {
                    type = KT_KEYUP;
                }

                if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP || message == WM_SYSCHAR)
                {
                    sysChar = true;
                }

                if (message == WM_IME_CHAR)
                {
                    imeChar = true;
                }

                _clientAdapter->GetCefBrowser()->SendKeyEvent(type, wParam.ToInt32(), lParam.ToInt32(), sysChar, imeChar);

                handled = true;
        }

        return IntPtr::Zero;
    }

    void CefWpfWebBrowser::SetBuffer(int width, int height, const CefRect& dirtyRect, const void* buffer)
    {
        _width = width;
        _height = height;
        if (buffer)
        {
            _buffer = (void *)buffer;
        }

        if (buffer != 0)
        {
            Dispatcher->Invoke(DispatcherPriority::Render,
                gcnew Action<WriteableBitmap^>(this, &CefWpfWebBrowser::SetBitmap), _bitmap);
        }
    }

    // XXX: don't know how to Invoke a parameterless delegate...
    void CefWpfWebBrowser::SetBitmap(WriteableBitmap^ bitmap)
    {
        int length = _width * _height * 4;

        if (length == 0)
        {
            return;
        }

        if (!_bitmap ||
            _bitmap->PixelWidth != _width ||
            _bitmap->PixelHeight != _height)
        {
            _image->Source = _bitmap = gcnew WriteableBitmap(_width, _height, 96 * _transform.M11, 96 * _transform.M22, PixelFormats::Bgr32, nullptr);
        }

        Int32Rect rect;
        rect.X = 0;
        rect.Y = 0;
        rect.Width = _width;
        rect.Height = _height;

        _bitmap->WritePixels(rect, (IntPtr)_buffer, length, _bitmap->BackBufferStride);
    }

    void CefWpfWebBrowser::Setup(HwndSource^ source, String^ address)
    {
        Focusable = true;
        FocusVisualStyle = nullptr;

        if (!CEF::IsInitialized)
        {
            throw gcnew InvalidOperationException("CEF is not initialized");
        }

        _address = address;
        _runJsFinished = gcnew AutoResetEvent(false);
        _browserInitialized = gcnew ManualResetEvent(false);
        _loadCompleted = gcnew RtzCountdownEvent();
        _transform = source->CompositionTarget->TransformToDevice;

        source->AddHook(gcnew Interop::HwndSourceHook(this, &CefWpfWebBrowser::SourceHook));

        HWND hWnd = static_cast<HWND>(source->Handle.ToPointer());
        CefWindowInfo window;
        window.SetAsOffScreen(hWnd);

        _clientAdapter = new WpfClientAdapter(this);
        CefRefPtr<WpfClientAdapter> ptr = _clientAdapter.get();

        CefBrowserSettings settings;
        CefBrowser::CreateBrowser(window, static_cast<CefRefPtr<CefClient>>(ptr), toNative(address), settings);

        EventArgs^ ea = gcnew EventArgs();
        SetupFinished(this, ea);
    }
}
