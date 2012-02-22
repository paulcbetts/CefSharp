#include "stdafx.h"
#pragma once

using namespace System;
using namespace System::IO;

namespace CefSharp
{
	//TODO: Maybe add implicit cookie support, expiration, etc. Do a bunch of the header work for users.
	public ref class Response
	{
	public:
		property int StatusCode { int get();  void set(int statusCode); }
		property String^ StatusText { String^ get();  void set(String^ statusText); }
		property String^ MimeType { String^ get(); void set(String^ mimeType); }
		property System::IO::Stream^ Stream { System::IO::Stream^ get(); }
		property long Length { long get(); }
		property System::Collections::Generic::Dictionary<String^, String^>^ Headers { System::Collections::Generic::Dictionary<String^, String^>^ get(); }

		Response();
		void SetStatus(int code, String^ text);
		void SetHeader(String^ name, String^ value);

	private:
		int _statusCode;
		String^ _statusText;
		String^ _mimeType;
		System::IO::Stream^ _stream;
		System::Collections::Generic::Dictionary<String^, String^>^ _headers;
	};
}