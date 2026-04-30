#include "WaitWindow.h"

#define WAIT_CLASS_NAME "WaitWindowClass"

bool waitWindow_Canceled = false;

WaitWindow::WaitWindow()
{
    m_hWnd = NULL;
    m_hInst = NULL;
}

WaitWindow::~WaitWindow()
{
    Close();
}

ATOM WaitWindow::Register(HINSTANCE hInst)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = WaitWindow::WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = WAIT_CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_WAIT);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    return RegisterClass(&wc);
}

bool WaitWindow::Create(HINSTANCE hInst, HWND hParent)
{
    m_hInst = hInst;

    Register(hInst);

    m_hWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_DLGMODALFRAME,
        WAIT_CLASS_NAME,
        "Aguarde...",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT,
        260, 120,
        hParent,
        NULL,
        hInst,
        NULL);

    waitWindow_Canceled = false;

    if (!m_hWnd)
        return false;

    int width = 200;
    int height = 20;

    // resolução da tela
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // cálculo da posição central
    int posX = (screenWidth - width) / 2;
    int posY = (screenHeight - height) / 2;

    CreateWindow(
        "STATIC",
        "Processando digitalização...",
        WS_CHILD | WS_VISIBLE,
        posX, posY, width, height,
        m_hWnd,
        NULL,
        hInst,
        NULL);

    return true;
}

void WaitWindow::Show()
{
    if (m_hWnd)
    {
        ShowWindow(m_hWnd, SW_SHOW);
        UpdateWindow(m_hWnd);
    }
}

void WaitWindow::Close()
{
    if (m_hWnd)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = NULL;
    }
}

LRESULT CALLBACK WaitWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
    {
        waitWindow_Canceled = true;
        DestroyWindow(hwnd);
        return 0; // impede fechar manualmente
    }
    case WM_DESTROY:
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
