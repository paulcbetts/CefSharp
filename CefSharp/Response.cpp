#include "stdafx.h"

#include "Response.h"

namespace CefSharp
{
	Response::Response() 
		: _statusCode(200)
		, _statusText("OK")
		,_mimeType("text/html")
		, _stream(nullptr)
		,_headers(nullptr)
	{
	}

	int Response::StatusCode::get() 
	{
		return (_statusCode);
	}

	void Response::StatusCode::set(int statusCode)
	{
		_statusCode = statusCode;
	}

	String^ Response::StatusText::get() 
	{
		return (_statusText);
	}

	void Response::StatusText::set(String^ statusText)
	{
		_statusText = statusText;
	}

	String^ Response::MimeType::get()
	{
		return(_mimeType);
	}

	void Response::MimeType::set(String^ mimeType)
	{
		_mimeType = mimeType;
	}

	System::IO::Stream^ Response::Stream::get()
	{
		if (_stream == nullptr)
		{
			_stream = gcnew MemoryStream();
		}
		return(_stream);
	}

	long Response::Length::get()
	{
		if (_stream ==nullptr)
		{
			return(0);
		}
		return(_stream->Length);
	}

	System::Collections::Generic::Dictionary<String^, String^>^ Response::Headers::get()
	{
		if (_headers == nullptr)
		{
			_headers = gcnew System::Collections::Generic::Dictionary<String^, String^>();
		}
		return(_headers);
	}

	void Response::SetStatus(int code, String^ text)
	{
		this->_statusCode = code;
		this->_statusText = text;
	}

	void Response::SetHeader(String^ name, String^ value)
	{
		Headers[name] = value;
	}


}

