// GuestBookEditor.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "GuestBookEditor.h"
#include "QuickPanel.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

bool is_click;
int current_x, current_y;

//UINT double_click_timer;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void OnPaint(HDC hdc);
void CALLBACK TimerProc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
    
    GdiplusStartupInput gdiplus_startup_input;
    ULONG_PTR gdiplus_token;

    GdiplusStartup(&gdiplus_token, &gdiplus_startup_input, NULL);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GUESTBOOKEDITOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    
    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GUESTBOOKEDITOR));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    GdiplusShutdown(gdiplus_token);
    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GUESTBOOKEDITOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GUESTBOOKEDITOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, 1280, 720, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static QuickPanel *quick_panel;

    TIMECAPS timecaps;
    timeGetDevCaps(&timecaps, sizeof(TIMECAPS));

    POINT mouse_position;

    switch (message)
    {
    case WM_CREATE:
        {
            quick_panel = new QuickPanel(hWnd);
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_LBUTTONUP:
        {
            quick_panel->MouseUp();
            is_click = false;
        }
        break;
    case WM_LBUTTONDOWN:
        {
            //timeKillEvent(double_click_timer);
            //double_click_timer = timeSetEvent(1, timecaps.wPeriodMax, TimerProc, (DWORD)hWnd, TIME_PERIODIC);

            mouse_position.x = LOWORD(lParam);
            mouse_position.y = HIWORD(lParam);
            
            quick_panel->MouseDown(mouse_position);

            is_click = true;
            current_x = LOWORD(lParam);
            current_y = HIWORD(lParam);
        }
        break;
    case WM_LBUTTONDBLCLK:
        {
            mouse_position.x = LOWORD(lParam);
            mouse_position.y = HIWORD(lParam);

            quick_panel->Open(mouse_position);
        }
        break;
    case WM_MOUSEMOVE:
        {
            mouse_position.x = LOWORD(lParam);
            mouse_position.y = HIWORD(lParam);

            quick_panel->MouseMove(mouse_position);

            if (is_click && quick_panel->IsOpen() == false)
            {
                HDC hdc;
                hdc = GetDC(hWnd);
                COLORREF as = RGB(quick_panel->GetR(), quick_panel->GetG(), quick_panel->GetB());
                HPEN n = CreatePen(PS_SOLID, (int)trunc(quick_panel->GetPenSize()), as);
                HPEN o = (HPEN)SelectObject(hdc, n);
                MoveToEx(hdc, current_x, current_y, NULL);
                LineTo(hdc, mouse_position.x, mouse_position.y);
                SelectObject(hdc, o);
                DeleteObject(n);
                ReleaseDC(hWnd, hdc);
                current_x = mouse_position.x;
                current_y = mouse_position.y;
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc, memDC;
            HBITMAP newBitmap, oldBitmap;
            RECT buffer;
            memDC = BeginPaint(hWnd, &ps);

            GetClientRect(hWnd, &buffer);
            hdc = CreateCompatibleDC(memDC);
            newBitmap = CreateCompatibleBitmap(memDC, buffer.right, buffer.bottom);
            oldBitmap = (HBITMAP)SelectObject(hdc, newBitmap);
            PatBlt(hdc, 0, 0, buffer.right, buffer.bottom, WHITENESS);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            
            OnPaint(hdc);
            quick_panel->Draw(hdc);
            
            GetClientRect(hWnd, &buffer);
            BitBlt(memDC, 0, 0, buffer.right, buffer.bottom, hdc, 0, 0, SRCCOPY);
            SelectObject(hdc, oldBitmap);
            DeleteObject(newBitmap);
            DeleteDC(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        delete quick_panel;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void OnPaint(HDC hdc)
{
}

// 비동기 타이머
void CALLBACK TimerProc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{

}
