#include "stdafx.h"
#pragma once

#include "CefSharp.h"
#include "WpfClientAdapter.h"
#include "ICefWebBrowser.h"
#include "ConsoleMessageEventArgs.h"
#include "RtzCountdownEvent.h"

using namespace Microsoft::Win32::SafeHandles;
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Windows;
using namespace System::Windows::Controls;
using namespace System::Windows::Input;
using namespace System::Windows::Interop;
using namespace System::Windows::Media;
using namespace System::Windows::Media::Imaging;
using namespace System::Windows::Threading;
using namespace System::Threading;

namespace CefSharp
{
    [TemplatePart(Name="PART_Browser", Type=System::Windows::Controls::Image::typeid)]
    public ref class CefWpfWebBrowser sealed : public ContentControl, ICefWebBrowser
    {
        bool _canGoForward;
        bool _canGoBack;
        bool _isLoading;
		bool _shouldOpenLinksInExternalBrowser;

        String^ _address;
        String^ _title;
        String^ _jsResult;
        bool _jsError;

        IBeforePopup^ _beforePopupHandler;
        IBeforeResourceLoad^ _beforeResourceLoadHandler;
        IBeforeMenu^ _beforeMenuHandler;
        IAfterResponse^ _afterResponseHandler;
        MCefRefPtr<WpfClientAdapter> _clientAdapter;

        AutoResetEvent^ _runJsFinished;
        RtzCountdownEvent^ _loadCompleted;
        ManualResetEvent^ _browserInitialized;

        Image^ _image;

        Matrix _transform;
        int _width, _height;
        void *_buffer;
        WriteableBitmap^ _bitmap;

		double _contentWidth;
		double _contentHeight;

        bool _mouseIsDown;
        CefBrowser::MouseButtonType _depressedMouseButton;

    private:
        void SetCursor(SafeFileHandle^ handle);
        IntPtr SourceHook(IntPtr hWnd, int message, IntPtr wParam, IntPtr lParam, bool% handled);
		void Setup(HwndSource^ source, String^ address);

    protected:
        virtual Size ArrangeOverride(Size size) override;
        virtual void OnGotFocus(RoutedEventArgs^ e) override;
        virtual void OnLostFocus(RoutedEventArgs^ e) override;
        virtual void OnMouseMove(System::Windows::Input::MouseEventArgs^ e) override;
        virtual void OnMouseLeave(System::Windows::Input::MouseEventArgs^ e) override;
        virtual void OnMouseWheel(MouseWheelEventArgs^ e) override;
        virtual void OnMouseDown(MouseButtonEventArgs^ e) override;
        virtual void OnMouseUp(MouseButtonEventArgs^ e) override;
        virtual void OnLostMouseCapture(MouseEventArgs^ e) override;

    public:
        CefWpfWebBrowser(HwndSource^ source, String^ address)
        {
			Setup(source, address);
        }

		CefWpfWebBrowser() {}

        virtual void OnApplyTemplate() override;

        void Load(String^ url);
        void Stop();
        void Back();
        void Forward();
        void Reload();
        void Reload(bool ignoreCache);
        void Print();
        String^ RunScript(String^ script, String^ scriptUrl, int startLine);
        String^ RunScript(String^ script, String^ scriptUrl, int startLine, int timeout);
		void UpdateContentSize(double width, double height);

        virtual void OnInitialized();

        virtual void SetTitle(String^ title);
        virtual void SetToolTip(String^ text);
        virtual void SetAddress(String^ address);
        virtual void SetNavState(bool isLoading, bool canGoBack, bool canGoForward);
        
        virtual void AddFrame(CefRefPtr<CefFrame> frame);
        virtual void FrameLoadComplete(CefRefPtr<CefFrame> frame);

        virtual void SetJsResult(String^ result);
        virtual void SetJsError();
        virtual void RaiseConsoleMessage(String^ message, String^ source, int line);

        virtual property IBeforePopup^ BeforePopupHandler
        {
            IBeforePopup^ get() { return _beforePopupHandler; }
            void set(IBeforePopup^ handler) { _beforePopupHandler = handler; }
        }

        virtual property IBeforeResourceLoad^ BeforeResourceLoadHandler
        {
            IBeforeResourceLoad^ get() { return _beforeResourceLoadHandler; }
            void set(IBeforeResourceLoad^ handler) { _beforeResourceLoadHandler = handler; }
        }

        virtual property IBeforeMenu^ BeforeMenuHandler
        {
            IBeforeMenu^ get() { return _beforeMenuHandler; }
            void set(IBeforeMenu^ handler) { _beforeMenuHandler = handler; }
        }

        virtual property IAfterResponse^ AfterResponseHandler
        {
            IAfterResponse^ get() { return _afterResponseHandler; }
            void set(IAfterResponse^ handler) { _afterResponseHandler = handler; }
        }

        property String^ Title
        {
            String^ get() { return _title; }
        }

        property String^ Address
        {
            String^ get() { return _address; }
        }

        property bool CanGoForward
        { 
            bool get() { return _canGoForward; } 
        }

        property bool CanGoBack
        { 
            bool get() { return _canGoBack; } 
        }

        property bool IsLoading
        {
            bool get() { return _isLoading; }
        }

		property double ContentWidth
		{
			double get() { return _contentWidth; }
		}

		property double ContentHeight
		{
			double get() { return _contentHeight; }
		}

        property bool IsInitialized
        {
            bool get()
            {
                return _clientAdapter.get() != nullptr && _clientAdapter->GetIsInitialized();
            }
        }

		property bool ShouldOpenLinksInExternalBrowser
		{
			virtual bool get() { return _shouldOpenLinksInExternalBrowser; }
			virtual void set(bool value) { _shouldOpenLinksInExternalBrowser = value; }
		}

		event EventHandler^ LoadFinished;
		event EventHandler^ SetupFinished;
        bool WaitForInitialized();

        virtual event PropertyChangedEventHandler^ PropertyChanged;

        // TODO: Initialized event can be subscribed by user code after actual Initialized event happens,
        // so we must handle this situation and in on add event handler raise event.
        // event EventHandler^ Initialized;

        event ConsoleMessageEventHandler^ ConsoleMessage;

        void SetCursor(CefCursorHandle cursor);
        void SetBuffer(int width, int height, const CefRect& dirtyRect, const void* buffer);
        void SetBitmap(WriteableBitmap^ bitmap);
    };
}
