// The MIT License (MIT)

// Copyright (c) Microsoft Corporation

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


/*
The ShellExecInExplorerProcess function can be used to start a process as the
non-administrator user while the current process is running as administrator.

Taken from https://github.com/microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/winui/shell/appplatform/ExecInExplorer/ExecInExplorer.cpp
*/


#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>

// Supposed to be in shlwapi.h
LWSTDAPI IUnknown_QueryService(
  IUnknown *punk,
  REFGUID  guidService,
  REFIID   riid,
  void     **ppvOut
);

// use the shell view for the desktop using the shell windows automation to find the
// desktop web browser and then grabs its view
//
// returns:
//      IShellView, IFolderView and related interfaces

static HRESULT GetShellViewForDesktop(REFIID riid, void **ppv)
{
    *ppv = NULL;

    IShellWindows *psw;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&psw));
    if (SUCCEEDED(hr))
    {
        HWND hwnd;
        IDispatch* pdisp;
        VARIANT vEmpty = {}; // VT_EMPTY
        if (S_OK == psw->FindWindowSW(&vEmpty, &vEmpty, SWC_DESKTOP, (long*)&hwnd, SWFO_NEEDDISPATCH, &pdisp))
        {
            IShellBrowser *psb;
            hr = IUnknown_QueryService(pdisp, SID_STopLevelBrowser, IID_PPV_ARGS(&psb));
            if (SUCCEEDED(hr))
            {
                IShellView *psv;
                hr = psb->QueryActiveShellView(&psv);
                if (SUCCEEDED(hr))
                {
                    hr = psv->QueryInterface(riid, ppv);
                    psv->Release();
                }
                psb->Release();
            }
            pdisp->Release();
        }
        else
        {
            hr = E_FAIL;
        }
        psw->Release();
    }
    return hr;
}

// From a shell view object gets its automation interface and from that gets the shell
// application object that implements IShellDispatch2 and related interfaces.

static HRESULT GetShellDispatchFromView(IShellView *psv, REFIID riid, void **ppv)
{
    *ppv = NULL;

    IDispatch *pdispBackground;
    HRESULT hr = psv->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&pdispBackground));
    if (SUCCEEDED(hr))
    {
        IShellFolderViewDual *psfvd;
        hr = pdispBackground->QueryInterface(IID_PPV_ARGS(&psfvd));
        if (SUCCEEDED(hr))
        {
            IDispatch *pdisp;
            hr = psfvd->get_Application(&pdisp);
            if (SUCCEEDED(hr))
            {
                hr = pdisp->QueryInterface(riid, ppv);
                pdisp->Release();
            }
            psfvd->Release();
        }
        pdispBackground->Release();
    }
    return hr;
}

HRESULT ShellExecInExplorerProcess(PCWSTR pszFile, PCWSTR pszArgs)
{
    IShellView *psv;
    HRESULT hr = GetShellViewForDesktop(IID_PPV_ARGS(&psv));
    if (SUCCEEDED(hr))
    {
        IShellDispatch2 *psd;
        hr = GetShellDispatchFromView(psv, IID_PPV_ARGS(&psd));
        if (SUCCEEDED(hr))
        {
            BSTR bstrFile = SysAllocString(pszFile);
            BSTR bstrArgs = SysAllocString(pszArgs);
            hr = bstrFile && bstrArgs ? S_OK : E_OUTOFMEMORY;
            if (SUCCEEDED(hr))
            {
                VARIANT vtEmpty = {}; // VT_EMPTY
                VARIANT vtArgs;
                vtArgs.vt = VT_BSTR;
                vtArgs.bstrVal = bstrArgs;
                hr = psd->ShellExecuteW(bstrFile, vtArgs, vtEmpty, vtEmpty, vtEmpty);
                SysFreeString(bstrFile);
                SysFreeString(bstrArgs);
            }
            psd->Release();
        }
        psv->Release();
    }
    return hr;
}
