#include "stdafx.h"
#include <CamLib/CamError.h>
#include <strsafe.h> // for StringCchPrintf

/////////////////////////////////////////////////////////////////////////////
// OnError
// Windows Error handler
// Gets the last windows error and then resets the error; gets the string
// associated with the error and displays a messagebox of the error
/////////////////////////////////////////////////////////////////////////////
void OnError(LPCSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code
    DWORD dwError = ::GetLastError();
    if (ERROR_SUCCESS == dwError)
    {
        return;
    }
    TRACE(_T("OnError: %s: %u\n"), lpszFunction, dwError);
    ::SetLastError(ERROR_SUCCESS); // reset the error

    LPVOID lpMsgBuf = nullptr;
    DWORD dwLen = ::FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, dwError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&lpMsgBuf), 0, nullptr);

    if (0 == dwLen)
    {
        TRACE(_T("OnError: FormatMessage error: %ud\n"), ::GetLastError());
        ::SetLastError(ERROR_SUCCESS); // reset the error
        return;
    }
    // Display the error message and exit the process
    auto lpDisplayBuf = static_cast<LPVOID>(::LocalAlloc(
        LMEM_ZEROINIT,
        (lstrlen(static_cast<LPCTSTR>(lpMsgBuf)) + lstrlen(static_cast<LPCTSTR>(lpszFunction)) + 40) * sizeof(TCHAR)));
    if (!lpDisplayBuf)
    {
        TRACE(_T("OnError: LocalAlloc error: %ud\n"), ::GetLastError());
        ::SetLastError(ERROR_SUCCESS); // reset the error
        ::LocalFree(lpMsgBuf);
        return;
    }
    HRESULT hr = StringCchPrintf(static_cast<LPTSTR>(lpDisplayBuf), ::LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                                 TEXT("%s failed with error %d: %s"), lpszFunction, dwError, lpMsgBuf);
    if (SUCCEEDED(hr))
    {
        //::MessageBox(0, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);
        TRACE(_T("OnError: : %s\n"), lpDisplayBuf);
    }
    else
    {
        TRACE(_T("OnError: StringCchPrintf error: %ud\n"), ::GetLastError());
        ::SetLastError(ERROR_SUCCESS); // reset the error
    }

    ::LocalFree(lpMsgBuf);
    ::LocalFree(lpDisplayBuf);
}

void ErrorMsg(const char *frmt, ...)
{
    DWORD written = 0;
    char buf[5000];
    va_list val;

    va_start(val, frmt);
    _vstprintf_s(buf, frmt, val); // Save replacement
    va_end(val);

    const COORD _80x50 = {80, 50};
    static BOOL startup = (AllocConsole(), SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), _80x50));
    WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buf, (DWORD)lstrlen(buf), &written, nullptr);
}

int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus)
{
    CString tstr("");
    CString mstr("");
    VERIFY(tstr.LoadString((UINT)strTitle));
    VERIFY(mstr.LoadString((UINT)strMsg));

    return ::MessageBox(hWnd, mstr, tstr, mbstatus);
}

int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val)
{
    CString tstr("");
    CString mstr("");
    CString fstr("");
    VERIFY(tstr.LoadString((UINT)strTitle));
    VERIFY(mstr.LoadString((UINT)strMsg));
    fstr.Format(mstr, val);

    return ::MessageBox(hWnd, fstr, tstr, mbstatus);
}

int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val1, long val2)
{
    CString tstr("");
    CString mstr("");
    CString fstr("");
    VERIFY(tstr.LoadString((UINT)strTitle));
    VERIFY(mstr.LoadString((UINT)strMsg));
    fstr.Format(mstr, val1, val2);

    return ::MessageBox(hWnd, fstr, tstr, mbstatus);
}
