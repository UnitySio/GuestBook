// GuestBookEditor.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "GuestBookEditor.h"
#include "mmsystem.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
int status; // 0 : 비활성화, 1 : 그리기, 2 : 지우기
std::vector<std::vector<POINT>> list;   // 그린 정보를 담을 vector
RECT erase = { 0, 0, 100, 100 };    // 지우개 버튼
HDC btndc;
HBITMAP btnBitmap;

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

    GdiplusStartupInput gdiplus_startup_input;
    ULONG_PTR gdiplus_token;

    GdiplusStartup(&gdiplus_token, &gdiplus_startup_input, NULL);

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

    GdiplusShutdown(gdiplus_token);
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
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

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
    POINT pos;
    RECT rect;
    switch (message)
    {
    case WM_CREATE:
    {
        hdc = GetDC(hWnd);

        btndc = CreateCompatibleDC(hdc);
        btnBitmap = CreateCompatibleBitmap(btndc, 100, 100);
        SelectObject(btndc, btnBitmap);
        Rectangle(btndc, 0, 0, 100, 100);
        TextOut(btndc, 25, 45, L"지우개", 3);
        BitBlt(hdc, 0, 0, 200, 200, btndc, 0, 0, SRCCOPY);

        ReleaseDC(hWnd, hdc);
        break;
    }
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
    case WM_LBUTTONDOWN:
    {
        status = 1; // 마우스를 그리기 상태로 활성화
        
        pos.x = GET_X_LPARAM(lParam);
        pos.y = GET_Y_LPARAM(lParam);

        std::vector<POINT> tempv;   // 임시 벡터
        list.push_back(tempv);
        list[list.size() - 1].push_back(pos);

        tempv.clear();  // 임시 벡터 제거
        std::vector<POINT>().swap(tempv);
        break;
    }
    case WM_LBUTTONUP:
    {
        status = 0; // 마우스 비활성화

        pos.x = GET_X_LPARAM(lParam);
        pos.y = GET_Y_LPARAM(lParam);

        if (PtInRect(&erase, pos))
        {
            status = 2; // 마우스의 상태를 지우개 상태로 변경

            for (int i = 0; i < list.size(); i++)
            {
                for (int j = 0; j < list[i].size(); j++)
                {
                    POINT erasepos = list[i].at(j);
                    if (erasepos.y - 5 <= pos.y && pos.y <= erasepos.y + 5 && erasepos.x - 5 <= pos.x && pos.x <= erasepos.x + 5)
                    {
                        // -999로 블랭크를 처리
                        (list[i])[j].x = -999;
                        (list[i])[j].y = -999;
                    }
                }
            }
        }
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (status == 1)    // 마우스가 그리기 상태라면
        {
            pos.x = GET_X_LPARAM(lParam);
            pos.y = GET_Y_LPARAM(lParam);
            list[list.size() - 1].push_back(pos);   // 좌표 값을 받아서 벡터에 추가
        }

        if (status == 2) // 지우개 상태라면
        {
            pos.x = GET_X_LPARAM(lParam);
            pos.y = GET_Y_LPARAM(lParam);

            // 전체 vector 리스트를 확인해서 지우는 좌표와 일치하는 값 제거하기
            for (int i = 0; i < list.size(); i++)
            {
                for (int j = 0; j < list[i].size(); j++)
                {
                    POINT erasepos = list[i].at(j);
                    if (erasepos.y - 5 <= pos.y && pos.y <= erasepos.y + 5 && erasepos.x - 5 <= pos.x && pos.x <= erasepos.x + 5)
                    {
                        (list[i])[j].x = -999;
                        (list[i])[j].y = -999;
                    }
                }
            }
        }
        // 그리기 갱신
        InvalidateRect(hWnd, NULL, FALSE);
        UpdateWindow(hWnd);
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        hdc = BeginPaint(hWnd, &ps);

        HDC memdc = CreateCompatibleDC(hdc);    // 메모리에 DC 생성
        GetClientRect(hWnd, &rect); // 현재 윈도우 창 크기를 받아오기

        HBITMAP memBitmap = CreateCompatibleBitmap(memdc, rect.right, rect.bottom); // 창 크기와 동일한 비트맵 생성
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memdc, memBitmap);    
        FillRect(memdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));    // 메모리 DC의 백그라운드 컬러를 흰색으로 설정

        // 벡터 list에 저장된 좌표를 통해서 그림 그리기
        for (int i = 0; i < list.size(); i++)
        {
            POINT st_pos = list[i].at(0);   // i번째 벡터에서 초기 값 받아오기

            // 이중반복문을 이용해서 vector의 다음값을 받아오기
            for (int j = 0; j < list[i].size(); j++)
            {
                POINT next_pos = list[i].at(j);
                // 트리거 추가
                if (st_pos.x == -999 && st_pos.y == -999 || next_pos.x == -999 && next_pos.y == -999)
                {
                    st_pos = next_pos;
                    continue;
                }
                MoveToEx(memdc, st_pos.x, st_pos.y, NULL);
                LineTo(memdc, next_pos.x, next_pos.y);
                st_pos = next_pos;  // 다음 좌표를 시작 좌표로 저장
            }
        }
        BitBlt(hdc, 0, 0, rect.right, rect.bottom, memdc, 0, 0, SRCCOPY);   // 메모리 DC에 그린 그림을 화면으로 복사(전달)
        BitBlt(hdc, 0, 0, rect.right, rect.bottom, btndc, 0, 0, SRCCOPY);

        // 생성 했던 메모리 공간 제거
        SelectObject(memdc, oldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memdc);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
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