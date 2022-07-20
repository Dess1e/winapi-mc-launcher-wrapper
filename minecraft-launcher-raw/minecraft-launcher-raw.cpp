// minecraft-launcher-raw.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "minecraft-launcher-raw.h"
#include <CommCtrl.h>
#include <Uxtheme.h>
#include "installer.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND parentWindow;
HWND progressBar;
HWND statusText;


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow
)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MINECRAFTLAUNCHERRAW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MINECRAFTLAUNCHERRAW));

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINECRAFTLAUNCHERRAW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MINECRAFTLAUNCHERRAW);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 350, 120, nullptr, nullptr, hInstance, nullptr);
    parentWindow = hWnd;
    SetWindowText(parentWindow, L"Launcher installer");

    if (!hWnd)
    {
        return FALSE;
    }

    HWND progBar = CreateWindowEx(0, PROGRESS_CLASS, 0, WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 10, 45, 315, 30, hWnd, NULL, hInstance, NULL);
    progressBar = progBar;
    HWND text = CreateWindow(L"Static", L"", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 10, 315, 30, hWnd, NULL, hInstance, NULL);
    statusText = text;

    SetWindowTheme(progBar, L"Explorer", NULL);


    SendMessage(progBar, PBM_SETRANGE, 0, MAKELPARAM(0, 4));
    SendMessage(progBar, PBM_SETSTEP, 1, 0);
   
    HDC hDC = GetDC(hWnd);
    int nHeight = -MulDiv(11, GetDeviceCaps(hDC, LOGPIXELSY), 72);
    HFONT hFont = hFont = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, _T("Segoe UI"));

    SendMessage(text, WM_SETFONT, (WPARAM)hFont, TRUE);

    SetWindowText(text, L"Starting...");

    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startInstaller, hWnd, 0, 0);

    UpdateWindow(hWnd);

    return TRUE;
}

void RedrawTextLabel() {
    RECT rect;
    GetClientRect(statusText, &rect);
    InvalidateRect(statusText, &rect, TRUE);
    MapWindowPoints(statusText, parentWindow, (POINT *) &rect, 2);
    RedrawWindow(parentWindow, &rect, NULL, RDW_ERASE | RDW_INVALIDATE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {

        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CTLCOLORSTATIC:
        {
           SetTextColor((HDC)wParam, RGB(0, 0, 0));
           SetBkMode((HDC)wParam, TRANSPARENT);
           return (LRESULT)GetStockObject(HOLLOW_BRUSH);
        }
        break;
    case WM_INSTALLER_UPDATE:
        {
            ShowWindow(hWnd, 1);
            SendMessage(progressBar, PBM_STEPIT, 0, 0);
            int param = (int)wParam;
            switch (param) {
            case 1:
                SetWindowText(statusText, L"Downloading JRE...");
                RedrawTextLabel();
                break;
            case 2:
                SetWindowText(statusText, L"Unpacking JRE...");
                RedrawTextLabel();
                break;
            case 3:
                SetWindowText(statusText, L"Downloading launcher binary...");
                RedrawTextLabel();
                break;
            case 4:
                exit(0);
            }
            UpdateWindow(hWnd);
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
