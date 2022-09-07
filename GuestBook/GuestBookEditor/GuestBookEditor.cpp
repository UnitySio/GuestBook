// GuestBookEditor.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "GuestBookEditor.h"
#include "QuickPanel.h"
#include "Timeline.h"

#include <vector>

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

double timer;

Timeline* timeline;

bool is_click;
int current_x, current_y;

struct P
{
    int current_x;
    int current_y;
    int x;
    int y;
    int width;
    COLORREF color;
    double time;
};

vector<P> v;

UINT drawing_timer;

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
    static QuickPanel* quick_panel;
    static TCHAR input[256];
    int input_length;

    TIMECAPS timecaps;
    timeGetDevCaps(&timecaps, sizeof(TIMECAPS));

    POINT mouse_position;

    switch (message)
    {
    case WM_CREATE:
        {
            quick_panel = new QuickPanel(hWnd);
            timeline = new Timeline(hWnd);
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_NEW_FILE:
                if (v.size() != 0)
                {
                    if (MessageBox(hWnd, L"저장하지 않은 내용은 지워집니다.", L"새 파일", MB_YESNO) == IDYES)
                    {
                        timer = 0;
                        v.clear();
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                }
                break;
            case IDM_SAVE:
                break;
            case IDM_LOAD:
                break;
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
    case WM_CHAR: // 키보드 입력(테스트)
        {
            RECT area = { 0, 0, 512, 32 };
            input_length = lstrlen(input);
            switch (wParam)
            {
            case 0x08: // 백스페이스
                if (input_length != 0)
                {
                    input[input_length] = wParam;
                    input[input_length - 1] = NULL;
                }
                break;
            case 0x0D: // 엔터
                break;
            case 0x09: // 탭
                break;
            case 0x1B: // ESC
                break;
            default:

                input[input_length] = wParam;
                input[input_length + 1] = NULL;
                break;
            }
            
            InvalidateRect(hWnd, &area, FALSE);
        }
        break;
    case WM_LBUTTONUP:
        {
            quick_panel->MouseUp();
            timeline->MouseUp();
            is_click = false;

            timeKillEvent(drawing_timer);
        }
        break;
    case WM_LBUTTONDOWN:
        {
            mouse_position.x = LOWORD(lParam);
            mouse_position.y = HIWORD(lParam);
            
            quick_panel->MouseDown(mouse_position);
            timeline->MouseDown(mouse_position);

            is_click = true;
            current_x = LOWORD(lParam);
            current_y = HIWORD(lParam);

            drawing_timer = timeSetEvent(1, timecaps.wPeriodMax, TimerProc, (DWORD)hWnd, TIME_PERIODIC);
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
            timeline->MouseMove(mouse_position);

            if (is_click && quick_panel->IsOpen() == false)
            {
                HDC hdc;
                hdc = GetDC(hWnd);
                COLORREF color = RGB(quick_panel->GetR(), quick_panel->GetG(), quick_panel->GetB());
                HPEN n = CreatePen(PS_SOLID, quick_panel->GetPenSize(), color);
                HPEN o = (HPEN)SelectObject(hdc, n);
                MoveToEx(hdc, current_x, current_y, NULL);
                LineTo(hdc, mouse_position.x, mouse_position.y);
                v.push_back({ current_x, current_y, mouse_position.x, mouse_position.y, quick_panel->GetPenSize(), color, timer });
                SelectObject(hdc, o);
                DeleteObject(n);
                ReleaseDC(hWnd, hdc);
                current_x = mouse_position.x;
                current_y = mouse_position.y;
                timeline->UpdateMaxTime(timer);
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

            TextOut(hdc, 0, 0, input, lstrlen(input));
            
            OnPaint(hdc);
            timeline->Draw(hdc);
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
        delete timeline;
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
    Graphics graphics(hdc);

    SolidBrush black_brush(Color(255, 0, 0, 0));

    FontFamily arial_font(L"Arial");
    Font font_style(&arial_font, 12, FontStyleRegular, UnitPixel);

    WCHAR header_word[1024];
    _stprintf_s(header_word, L"Timer: %.3lfs", timer);

    PointF header_font_position(10, 10);
    graphics.DrawString(header_word, -1, &font_style, header_font_position, &black_brush);

    for (int i = 0; i < v.size(); i++)
    {
        HPEN n = CreatePen(PS_SOLID, v[i].width, v[i].color);
        HPEN o = (HPEN)SelectObject(hdc, n);
        MoveToEx(hdc, v[i].current_x, v[i].current_y, NULL);
        LineTo(hdc, v[i].x, v[i].y);
        SelectObject(hdc, o);
        DeleteObject(n);
    }
}

// 비동기 타이머
void CALLBACK TimerProc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    if (uTimerID == drawing_timer)
    {
        timer += 0.001;
        InvalidateRect((HWND)dwUser, NULL, FALSE);
    }
}
