#include "Window.h"

// ��� ���� �ʱ�ȭ
unique_ptr<Window> Window::instance_ = nullptr;
once_flag Window::flag_;

// â Ŭ������ ���
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

// �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����
BOOL Window::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        0, 0, 1600, 900, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    DragAcceptFiles(hWnd, TRUE);

    return TRUE;
}

// �� â�� �޽����� ó��
LRESULT Window::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return instance_->WndProc(hWnd, message, wParam, lParam);
}

LRESULT Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TIMECAPS timecaps;
    timeGetDevCaps(&timecaps, sizeof(TIMECAPS));

    POINT mouse_position;

    switch (message)
    {
    case WM_CREATE:
    {
        control_ = make_unique<Control>(hWnd);
        timeline_ = make_unique<Timeline>(hWnd);
        file_manager_ = make_unique<FileManager>(hWnd);
        canvas_ = make_unique<Canvas>(hWnd);
        quick_panel_ = make_unique<QuickPanel>(hWnd);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // �޴� ������ ���� �м��մϴ�:
        switch (wmId)
        {
        case IDM_NEW_FILE:
            canvas_->Reset();
            timer_ = 0;
            timeline_->UpdateMaxTime(0);
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        case IDM_SAVE:
            canvas_->OpenSaveFile();
            break;
        case IDM_LOAD:
            canvas_->OpenLoadFile();
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
        // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�...

        canvas_->Draw(hdc);
        file_manager_->Draw(hdc);
        control_->Draw(hdc);
        timeline_->Draw(hdc);
        OnPaint(hdc);
        quick_panel_->Draw(hdc);

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

        quick_panel_->MouseUp();
        timeline_->MouseUp();
        canvas_->MouseUp();
        file_manager_->MouseUp();

        timeKillEvent(drawing_timer_);
        drawing_timer_ = NULL;
    }
    break;
    case WM_LBUTTONDOWN:
    {
        mouse_position.x = LOWORD(lParam);
        mouse_position.y = HIWORD(lParam);

        quick_panel_->MouseDown(mouse_position);

        if (!quick_panel_->OnOpen() and !timeline_->OnPlay())
        {
            control_->MouseDown(mouse_position);
            timeline_->MouseDown(mouse_position);
            canvas_->MouseDown(mouse_position);
            file_manager_->MouseDown(mouse_position);
        }
    }

    break;
    case WM_LBUTTONDBLCLK:
    {
        mouse_position.x = LOWORD(lParam);
        mouse_position.y = HIWORD(lParam);

        if (!timeline_->OnPlay())
        {
            quick_panel_->Open(mouse_position);

            if (!quick_panel_->OnOpen())
            {
                file_manager_->MouseDoubleDown(mouse_position);
            }
        }
    }
    break;
    case WM_MOUSEMOVE:
    {
        mouse_position.x = LOWORD(lParam);
        mouse_position.y = HIWORD(lParam);

        timeline_->MouseMove(mouse_position);
        file_manager_->MouseMove(mouse_position);
        canvas_->MouseMove(mouse_position, quick_panel_->GetPenSize(), timer_, quick_panel_->GetRGB());
        quick_panel_->MouseMove(mouse_position);


        if (canvas_->OnCanvasClick())
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
    case WM_MOUSEWHEEL:
    {
        file_manager_->MouseWheel((float)((short)HIWORD(wParam)) / WHEEL_DELTA);
        InvalidateRect(hWnd, NULL, FALSE);
    }
    break;
    case WM_DROPFILES:
    {
        HDROP hDrop = (HDROP)wParam;

        WCHAR drag_file_path[256] = L"";
        WCHAR drag_file_name[256] = L"";
        WCHAR file_path[256] = L"";

        UINT count = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

        for (UINT i = 0; i < count; i++)
        {
            DragQueryFile(hDrop, i, drag_file_path, 256);

            if (fs::is_directory(drag_file_path))
            {
                for (int j = 0; j < wcslen(drag_file_path); j++)
                {
                    if (drag_file_path[j] == L'\\')
                    {
                        wsprintf(drag_file_name, L"%s", drag_file_path + j + 1);
                    }
                }

                wsprintf(file_path, L"%s\\%s", file_manager_->GetCurrentPath(), drag_file_name);
                fs::create_directory(file_path);
            }
            else if (fs::is_regular_file(drag_file_path))
            {
                wsprintf(file_path, L"%s", file_manager_->GetCurrentPath());
            }

            fs::copy(drag_file_path, file_path, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
        }

        DragFinish(hDrop);
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

// ���� ��ȭ ������ �޽��� ó����
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
        InvalidateRect(window->hWnd, NULL, FALSE);
    }

    ReleaseDC(window->hWnd, hdc);
}

void Window::OnPaint(HDC hdc)
{
    Graphics graphics(hdc);
}

void Window::SetTimer(int time)
{
    timer_ = time;
}

Window* Window::GetInstance()
{
    call_once(flag_, []() // ���ٽ�
        {
            instance_.reset(new Window);
        });

    return instance_.get();
}

Control* Window::GetControl()
{
    return control_.get();
}

Timeline* Window::GetTimeline()
{
    return timeline_.get();
}

FileManager* Window::GetFileManager()
{
    return file_manager_.get();
}

Canvas* Window::GetCanvas()
{
    return canvas_.get();
}

QuickPanel* Window::GetQuickPanel()
{
    return quick_panel_.get();
}
