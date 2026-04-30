#pragma once
#include <wtypes.h>

extern bool waitWindow_Canceled;

class WaitWindow
{
public:
    WaitWindow();
    ~WaitWindow();

    bool Create(HINSTANCE hInst, HWND hParent);
    void Show();
    void Close();

private:
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    static ATOM Register(HINSTANCE hInst);

    HWND m_hWnd;
    HINSTANCE m_hInst;
};
