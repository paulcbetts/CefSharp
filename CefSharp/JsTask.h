#include "stdafx.h"
#pragma once

#include "ICefWebBrowser.h"

namespace CefSharp
{
	/* TODO: This relies on the libcef patch in the patches folder
    class JsTask : public CefV8Task
    {
        gcroot<ICefWebBrowser^> _browser;
        CefString _script;
        CefString _scriptName;
        int _lineNo;
    public:
        JsTask(ICefWebBrowser^ browser, CefString script, CefString scriptName, int lineNo)
            : _browser(browser), _script(script), _scriptName(scriptName), _lineNo(lineNo)
        {
        }

        virtual CefString GetScript() { return _script; };
        virtual CefString GetScriptName() { return _scriptName; };
        virtual int GetStartLine() { return _lineNo; };
        virtual void HandleSuccess(CefRefPtr<CefV8Value> result);
        virtual void HandleError();

        IMPLEMENT_REFCOUNTING(JsTask);
    };
*/
}