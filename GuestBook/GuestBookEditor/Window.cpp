#include "Window.h"

// 멤버 변수 초기화
Window* Window::instance_ = nullptr;

// 창 클래스를 등록
ATOM Window::MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc = StaticWndProc;
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

// 인스턴스 핸들을 저장하고 주 창을 만듬
BOOL Window::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        0, 0, 1280, 720, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// 주 창의 메시지를 처리
LRESULT Window::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return instance_->WndProc(hWnd, message, wParam, lParam);
}

LRESULT Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static QuickPanel* quick_panel;

    TIMECAPS timecaps;
    timeGetDevCaps(&timecaps, sizeof(TIMECAPS));
    
    POINT mouse_position;

    switch (message)
    {
    case WM_CREATE:
    {
        GetClientRect(hWnd, &client_area_);
        window_area_ = { 0, 0, client_area_.right - client_area_.left, client_area_.bottom - client_area_.top };

        quick_panel = new QuickPanel(hWnd);
        timeline_ = new Timeline(hWnd);
        canvas_ = new Canvas(hWnd, window_area_.right - 100, 300);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_NEW_FILE:
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
    case WM_PAINT:
    {
        GetClientRect(hWnd, &client_area_);
        window_area_ = { 0, 0, client_area_.right - client_area_.left, client_area_.bottom - client_area_.top };

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

        canvas_->Draw(hdc);
        OnPaint(hdc);
        timeline_->Draw(hdc);
        quick_panel->Draw(hdc);

        GetClientRect(hWnd, &buffer);
        BitBlt(memDC, 0, 0, buffer.right, buffer.bottom, hdc, 0, 0, SRCCOPY);
        SelectObject(hdc, oldBitmap);
        DeleteObject(newBitmap);
        DeleteDC(hdc);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_LEFT:
            timeline_->Play();

            if (play_timer_ == NULL)
            
            {
                play_timer_ = timeSetEvent(1, timecaps.wPeriodMax, TimerProc, (DWORD_PTR)this, TIME_PERIODIC);
            }
            else
            {
                timeKillEvent(play_timer_);
                play_timer_ = NULL;
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
        }
    }
    break;
    case WM_LBUTTONUP:
    {
        mouse_position.x = LOWORD(lParam);
        mouse_position.y = HIWORD(lParam);
        quick_panel->MouseUp();
        //timeline_->MouseUp();
        canvas_->MouseUp();

        timeKillEvent(drawing_timer_);
        drawing_timer_ = NULL;
    }
    break;
    case WM_LBUTTONDOWN:
    {
        mouse_position.x = LOWORD(lParam);
        mouse_position.y = HIWORD(lParam);

        quick_panel->MouseDown(mouse_position);

        if (!quick_panel->IsOpen() and !timeline_->IsPlaying())
        {
            //timeline_->MouseDown(mouse_position);
            canvas_->MouseDown(mouse_position);
        }
    }
    break;
    case WM_LBUTTONDBLCLK:
    {
        mouse_position.x = LOWORD(lParam);
        mouse_position.y = HIWORD(lParam);

        if (!timeline_->IsPlaying())
        {
            quick_panel->Open(mouse_position);
        }
    }
    break;
    case WM_MOUSEMOVE:
    {
        mouse_position.x = LOWORD(lParam);
        mouse_position.y = HIWORD(lParam);

        quick_panel->MouseMove(mouse_position);
        //timeline_->MouseMove(mouse_position);

        canvas_->MouseMove(mouse_position, quick_panel->GetPenSize(), timer_, quick_panel->GetRGB());

        if (canvas_->IsCanvasClick())
        {
            if (drawing_timer_ == NULL)
            {
                drawing_timer_ = timeSetEvent(1, timecaps.wPeriodMax, TimerProc, (DWORD_PTR)this, TIME_PERIODIC);
            }

            timeline_->UpdateMaxTime(timer_);

        }
        else
        {
            timeKillEvent(drawing_timer_);
            drawing_timer_ = NULL;
        }
    }
    break;
    case WM_DESTROY:
        delete quick_panel;
        delete timeline_;
        delete canvas_;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기
INT_PTR CALLBACK Window::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

void CALLBACK Window::TimerProc(UINT m_nTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    RECT area = { 0, 0, 500, 30 };
    Window* window = (Window*)dwUser;

    HDC hdc;
    hdc = GetDC(window->hWnd);

    if (m_nTimerID == window->drawing_timer_)
    {
        window->timer_ += 0.001;
        InvalidateRect(window->hWnd, &area, FALSE);
    }

    if (m_nTimerID == window->play_timer_)
    {
        window->timeline_->AddTime(0.001);

        for (int i = 0; i < window->canvas_->GetPoints().size(); i++)
        {
            if ((int)trunc(window->canvas_->GetPoints()[i].time * 1000) == window->timeline_->GetTime())
            {
                HPEN n = CreatePen(PS_SOLID, window->canvas_->GetPoints()[i].width, window->canvas_->GetPoints()[i].color);
                HPEN o = (HPEN)SelectObject(hdc, n);
                MoveToEx(hdc, window->canvas_->GetPoints()[i].start_x, window->canvas_->GetPoints()[i].start_y, NULL);
                LineTo(hdc, window->canvas_->GetPoints()[i].end_x, window->canvas_->GetPoints()[i].end_y);
                SelectObject(hdc, o);
                DeleteObject(n);
            }
        }
    }

    ReleaseDC(window->hWnd, hdc);
}

void Window::OnPaint(HDC hdc)
{
    if (timeline_->IsPlaying() == false)
    {
        canvas_->UpdateDraw(hdc);
    }
}

Window::Window() : hInst(NULL), hWnd(NULL) // 멤버 변수 리스트 초기화
{
}

Window::~Window()
{
}

void Window::Create()
{
	if (!instance_)
	{
		instance_ = new Window();
	}
}

Window* Window::GetInstance()
{
    return instance_;
}
