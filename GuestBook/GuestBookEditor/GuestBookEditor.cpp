    // GuestBookEditor.cpp : 애플리케이션에 대한 진입점을 정의합니다.
    //

#include "framework.h"
#include "GuestBookEditor.h"
#include "Button.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
// 그리기 상태를 파악하기 위한 변수

int status;      // 0 : 비활성화, 1 : 그리기ON, 2 : 지우개ON
HDC memdc;      // 메모리 DC 값
POINT st_pos;   // 시작 POINT좌표
HBITMAP memBitmap;  // 메모리 DC에서 사용할 Bitmap 값
RECT eraser = { 0, 0, 100, 100 };
RECT clear = { 100, 0, 200, 200 };

// 윈도우 크기
const int windows_size_width = 1280;
const int windows_size_height = 720;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GUESTBOOKEDITOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
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

    return (int)msg.wParam;
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

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GUESTBOOKEDITOR));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GUESTBOOKEDITOR);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
    hInst = hInstance;       // 인스턴스 핸들을 전역 변수에 저장합니다.

    /*HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);*/
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_SYSMENU,
        0, 0, windows_size_width, windows_size_height, nullptr, nullptr, hInstance, nullptr);

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
    HDC hdc;
    switch (message)
    {
    case WM_CREATE:
    {
        // memBitmap 그리고 memDC 생성하기 위한 과정
        hdc = GetDC(hWnd);

        memdc = CreateCompatibleDC(hdc);
        RECT rect;
        GetClientRect(hWnd, &rect);     // 현재 윈도우 창 크기 받아오기
        memBitmap = CreateCompatibleBitmap(memdc, rect.right, rect.bottom);     // 사용자의 화면과 같은 크기의 비트맵 생성
        SelectObject(memdc, memBitmap);
        FillRect(memdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));    // 화면 전체를 흰색으로 칠하기
        BitBlt(hdc, 0, 0, rect.right, rect.bottom, memdc, 0, 0, SRCCOPY);   // 원본 DC인 memdc의 비트맵을 사본 DC인 hdc에 그대로 복사.

        ReleaseDC(hWnd, hdc);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        // 시작 좌표 저장
        st_pos.x = GET_X_LPARAM(lParam);
        st_pos.y = GET_Y_LPARAM(lParam);

        // 마우스 상태 결정
        status = Button(hWnd, lParam, memdc, status, eraser, clear, st_pos);

        /*
        if (PtInRect(&eraser, st_pos))
        {
            //status = 0;
            MessageBox(hWnd, L"지우개 활성화", L"알림", MB_OK);

            //마우스 상태 활성화
            status = 2;

            //지우는 좌표 받아오기
            st_pos.x = GET_X_LPARAM(lParam);
            st_pos.y = GET_Y_LPARAM(lParam);
        }
        else if (PtInRect(&clear, st_pos))
        {
            MessageBox(hWnd, L"화면 전체 지우기", L"알림", MB_OK);
            RECT rect;
            GetClientRect(hWnd, &rect);
            FillRect(memdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
        }
        */
        break;
    }
    case WM_LBUTTONUP:
    {
        status = 0;
        break;
    }
    /*case WM_RBUTTONDOWN:
    {
            //마우스 상태 활성화
            status = 2;

            //지우는 좌표 받아오기
            st_pos.x = GET_X_LPARAM(lParam);
            st_pos.y = GET_Y_LPARAM(lParam);
        break;
    }
    */
    case WM_RBUTTONUP:
    {
        status = 0;
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (status == 1)    // 그리기 상태
        {
            // 좌표를 받아서 값을 추가합니다.
            POINT pos;
            pos.x = GET_X_LPARAM(lParam);
            pos.y = GET_Y_LPARAM(lParam);

            RECT rect;
            GetClientRect(hWnd, &rect);
            hdc = GetDC(hWnd);

            HBITMAP oldBitmap = (HBITMAP)SelectObject(memdc, memBitmap);
            MoveToEx(memdc, st_pos.x, st_pos.y, NULL);
            LineTo(memdc, pos.x, pos.y);
            BitBlt(hdc, 0, 0, rect.right, rect.bottom, memdc, 0, 0, SRCCOPY);

            ReleaseDC(hWnd, hdc);
            st_pos.x = pos.x;
            st_pos.y = pos.y;
        }

        // 지우개 상태
        if (status == 2)
        {
            POINT pos;
            pos.x = GET_X_LPARAM(lParam);
            pos.y = GET_Y_LPARAM(lParam);

            RECT rect;
            GetClientRect(hWnd, &rect);

            hdc = GetDC(hWnd);

            HBITMAP oldBitmap = (HBITMAP)SelectObject(memdc, memBitmap);
            //그리기 색상 설정
            HPEN newPen = CreatePen(PS_SOLID, 10, RGB(255, 255, 255));      // 팬 객채 생성(흰 색)
            HPEN oldPen = (HPEN)SelectObject(memdc, newPen);

            MoveToEx(memdc, st_pos.x, st_pos.y, NULL);
            LineTo(memdc, pos.x, pos.y);
            BitBlt(hdc, 0, 0, rect.right, rect.bottom, memdc, 0, 0, SRCCOPY);

            SelectObject(memdc, oldPen);
            DeleteObject(newPen);

            ReleaseDC(hWnd, hdc);
            st_pos.x = pos.x;
            st_pos.y = pos.y;
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        hdc = BeginPaint(hWnd, &ps);
        RECT rect;

        GetClientRect(hWnd, &rect);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memdc, memBitmap);        // 비트 패턴을 저장하기 위한 변수
        BitBlt(hdc, 0, 0, rect.right, rect.bottom, memdc, 0, 0, SRCCOPY);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
    {
        DeleteObject(memBitmap);
        DeleteDC(memdc);
        PostQuitMessage(0);
        break;
    }

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
