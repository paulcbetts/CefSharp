#include "stdafx.h"
#pragma once

#include "Request.h"
#include "Response.h"

using namespace System;
using namespace System::IO;

namespace CefSharp {

 public interface class ISchemeHandler
{
    /// <summary>
    /// if request is handled return true and set response properties accordingly.
    /// if no data the leave stream null
    /// </summary>
    bool ProcessRequest(IRequest^ request, Response^% response);
};

public interface class ISchemeHandlerFactory
{
    ISchemeHandler^ Create();
};

class SchemeHandlerWrapper : public CefSchemeHandler
{
    gcroot<ISchemeHandler^> _handler;
    gcroot<Response^> _response;

public:

    SchemeHandlerWrapper(ISchemeHandler^ handler) : _handler(handler) 
    {
        if(!_handler)
        {
            throw gcnew ArgumentException("handler must not be null");
        }
    }

    virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefSchemeHandlerCallback> callback);
    virtual void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl);
    virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefSchemeHandlerCallback> callback);
    virtual void Cancel();

    IMPLEMENT_LOCKING(SchemeHandlerWrapper);
    IMPLEMENT_REFCOUNTING(SchemeHandlerWrapper);
};

class SchemeHandlerFactoryWrapper : public CefSchemeHandlerFactory
{
    gcroot<ISchemeHandlerFactory^> _factory;
   
public:
    SchemeHandlerFactoryWrapper(ISchemeHandlerFactory^ factory) 
        : _factory(factory) {}

    virtual CefRefPtr<CefSchemeHandler> Create(CefRefPtr<CefBrowser> browser, const CefString& scheme_name, CefRefPtr<CefRequest> request);

    IMPLEMENT_REFCOUNTING(SchemeHandlerFactoryWrapper);
};

};
