#include "Window.h"

// 멤버 변수 초기화
unique_ptr<Window> Window::instance_ = nullptr;
once_flag Window::flag_;

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

// 주 창의 메시지를 처리
LRESULT Window::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return instance_->WndProc(hWnd, message, wParam, lParam);
}

LRESULT Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static unique_ptr<QuickPanel> quick_panel;

    TIMECAPS timecaps;
    timeGetDevCaps(&timecaps, sizeof(TIMECAPS));
    
    POINT mouse_position;

    switch (message)
    {
    case WM_CREATE:
    {
        quick_panel = make_unique<QuickPanel>(hWnd);
        timeline_ = make_unique<Timeline>(hWnd);
        canvas_ = make_unique<Canvas>(hWnd, 1000, 500);
        file_manager_ = make_unique<FileManager>(hWnd);

        LoadGIF(L"Resources/PlayIcon.gif");

        GUID guid = FrameDimensionTime;
        image_->SelectActiveFrame(&guid, current_frame_);
        frame_timer_ = timeSetEvent(((UINT*)property_item_[0].value)[current_frame_] * 5, timecaps.wPeriodMax, TimerProc, (DWORD_PTR)this, TIME_ONESHOT);
        ++current_frame_;
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
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
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...

        canvas_->Draw(hdc);
        OnPaint(hdc);
        timeline_->Draw(hdc);
        file_manager_->Draw(hdc);
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
        file_manager_->MouseUp();

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
            file_manager_->MouseDown(mouse_position);
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
            file_manager_->MouseDoubleDown(mouse_position);
        }
    }
    break;
    case WM_MOUSEMOVE:
    {
        mouse_position.x = LOWORD(lParam);
        mouse_position.y = HIWORD(lParam);

        quick_panel->MouseMove(mouse_position);
        //timeline_->MouseMove(mouse_position);
        file_manager_->MouseMove(mouse_position);

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
        // 수동으로 메모리 해제 시 사용(일반적으로는 자동으로 메모리를 해제)
        /*timeline_.reset();
        canvas_.reset();
        quick_panel.reset();*/

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

    TIMECAPS timecaps;
    timeGetDevCaps(&timecaps, sizeof(TIMECAPS));

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

    if (m_nTimerID == window->frame_timer_)
    {
        RECT area = { 0, 0, 100, 100 };

        GUID guid = FrameDimensionTime;
        window->image_->SelectActiveFrame(&guid, window->current_frame_);

        window->frame_timer_ = timeSetEvent(((UINT*)window->property_item_[0].value)[window->current_frame_] * 5, timecaps.wPeriodMax, TimerProc, (DWORD_PTR)window, TIME_ONESHOT);
        
        window->current_frame_ = (++window->current_frame_) % (window->frame_count_);

        InvalidateRect(window->hWnd, &area, FALSE);
    }

    ReleaseDC(window->hWnd, hdc);
}

void Window::OnPaint(HDC hdc)
{
    Graphics graphics(hdc);

    graphics.DrawImage(image_, 0, 0, 100, 100);

    if (timeline_->IsPlaying() == false)
    {
        for (int i = 0; i < canvas_->GetPoints().size(); i++)
        {
            canvas_->DrawLine(hdc, i);
        }
    }
    else
    {
        /*아래와 같은 방식으로 계속해서 다시 그리는 방식을 선택한 이유는
        순차적인 탐색을 하면서 조금씩 조금씩 그리는 경우 비용이 비싸며,
        현재 사용하고 있는 타이머의 구조상 재시간안에 모두 실행할 수 없어
        문제가 발생하기 때문에 아래와 같은 방식을 사용하였다.*/

        for (int i = 0; i < canvas_->GetPoints().size(); i++)
        {
            if ((int)trunc(canvas_->GetPoints()[i].time * 1000) <= timeline_->GetTime())
            {
                canvas_->DrawLine(hdc, i);
            }
        }
    }
}

void Window::LoadGIF(LPCTSTR file_name)
{
    image_ = new Image(file_name);

    UINT count = image_->GetFrameDimensionsCount();

    dimension_ids_ = new GUID[count];
    image_->GetFrameDimensionsList(dimension_ids_, count);

    WCHAR guid[39];
    StringFromGUID2(dimension_ids_[0], guid, 39);
    frame_count_ = image_->GetFrameCount(&dimension_ids_[0]);

    UINT size = image_->GetPropertyItemSize(PropertyTagFrameDelay);
    property_item_ = (PropertyItem*)malloc(size);
    image_->GetPropertyItem(PropertyTagFrameDelay, size, property_item_);
}

void Window::SetTimer(int time)
{
    timer_ = time;
}

Window* Window::GetInstance()
{
    call_once(flag_, []() // 람다식
        {
            instance_.reset(new Window);
        });

    return instance_.get();
}

Timeline* Window::GetTimeline()
{
    return timeline_.get();
}

Canvas* Window::GetCanvas()
{
    return canvas_.get();
}

FileManager* Window::GetFileManager()
{
    return file_manager_.get();
}
