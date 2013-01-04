#include "stdafx.h"

#include "SchemeHandler.h"

namespace CefSharp
{
    void SchemeHandlerWrapper::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
    {
        if (_response)
		{
			if (_response->MimeType)
			{
				response->SetMimeType(toNative(_response->MimeType));
			}
			response->SetStatus(_response->StatusCode);
			response_length = _response->Length;

			if (_response->Headers)
			{
				//if (_response->Headers.Length > 0)
				{
					// TODO: foreach header...
				}
			}
		}
    }

    bool SchemeHandlerWrapper::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefSchemeHandlerCallback> callback)
    {
        bool handled = false;
 
        AutoLock lock_scope(this);

		Response^ response = gcnew Response();
        IRequest^ requestWrapper = gcnew CefRequestWrapper(request);
        if (_handler->ProcessRequest(requestWrapper, response))
        {
			_response = response;
            callback->HeadersAvailable();

            handled = true;
        }

        return handled;
    }

    bool SchemeHandlerWrapper::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefSchemeHandlerCallback> callback)
    {
        bool has_data = false;

        AutoLock lock_scope(this);

        if(_response)
        {
			int readBytes = bytes_to_read >= _response->Stream->Length ? bytes_to_read : (int)_response->Stream->Length;
			
			_response->Stream->Seek(0, SeekOrigin::Begin);
            
			array<Byte>^ buffer = gcnew array<Byte>(readBytes);
			int ret = _response->Stream->Read(buffer, 0, readBytes);
            
			pin_ptr<Byte> src = &buffer[0];
            memcpy(data_out, static_cast<void*>(src), ret);
            bytes_read = ret;
            has_data = true;
        }

        return has_data;
    }

    void SchemeHandlerWrapper::Cancel()
    {
        _response = nullptr;
    }

    CefRefPtr<CefSchemeHandler> SchemeHandlerFactoryWrapper::Create(CefRefPtr<CefBrowser> browser, const CefString& scheme_name, CefRefPtr<CefRequest> request)
    {
        ISchemeHandler^ handler = _factory->Create();
        CefRefPtr<SchemeHandlerWrapper> wrapper = new SchemeHandlerWrapper(handler);
        return static_cast<CefRefPtr<CefSchemeHandler>>(wrapper);
    }
}
