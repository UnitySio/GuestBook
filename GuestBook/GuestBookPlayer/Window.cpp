#include "pch.h"
#include "Window.h"

using namespace std;
using namespace Gdiplus;

// ��� ���� �ʱ�ȭ
unique_ptr<Window> Window::instance_ = nullptr;
once_flag Window::flag_;

TIMECAPS timecaps;

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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GUESTBOOKPLAYER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GUESTBOOKPLAYER);
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
    POINT mouse_position;

    switch (message)
    {
    case WM_CREATE:
    {
        UpdateWindowArea();

        file_manager_ = make_unique<FileManager>(hWnd);
        canvas_ = make_unique<Canvas>(hWnd);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // �޴� ������ ���� �м��մϴ�:
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
    case WM_PAINT:
    {
        UpdateWindowArea();

        PAINTSTRUCT ps;
        HDC hdc, memDC;
        HBITMAP new_bitmap, old_bitmap;
        memDC = BeginPaint(hWnd, &ps);
        new_bitmap = CreateCompatibleBitmap(memDC, window_area_.right, window_area_.bottom);
        hdc = CreateCompatibleDC(memDC);
        old_bitmap = (HBITMAP)SelectObject(hdc, new_bitmap);
        DeleteObject(old_bitmap);
        PatBlt(hdc, 0, 0, window_area_.right, window_area_.bottom, WHITENESS);
        // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�...

        canvas_->Draw(hdc);
        file_manager_->Draw(hdc);
        OnPaint(hdc);

        BitBlt(memDC, 0, 0, window_area_.right, window_area_.bottom, hdc, 0, 0, SRCCOPY);
        DeleteDC(hdc);
        DeleteObject(new_bitmap);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_LBUTTONUP:
    {
        mouse_position.x = LOWORD(lParam);
        mouse_position.y = HIWORD(lParam);

        file_manager_->MouseUp();
    }
    break;
    case WM_LBUTTONDOWN:
    {
        mouse_position.x = LOWORD(lParam);
        mouse_position.y = HIWORD(lParam);

        file_manager_->MouseDown(mouse_position);
    }

    break;
    case WM_LBUTTONDBLCLK:
    {
        mouse_position.x = LOWORD(lParam);
        mouse_position.y = HIWORD(lParam);

        file_manager_->MouseDoubleDown(mouse_position);
    }
    break;
    case WM_MOUSEMOVE:
    {
        mouse_position.x = LOWORD(lParam);
        mouse_position.y = HIWORD(lParam);

        file_manager_->MouseMove(mouse_position);
    }
    break;
    case WM_MOUSEWHEEL:
    {
        // WM_MOUSEWHEEL �������� ���� ��ġ�� ��ȯ
        mouse_position.x = LOWORD(lParam);
        mouse_position.y = HIWORD(lParam);
        ScreenToClient(hWnd, &mouse_position); // ��� ��ġ�� ���ϱ� ���� ���

        file_manager_->MouseWheel(mouse_position, (float)((short)HIWORD(wParam)) / WHEEL_DELTA);
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
    Window* window = (Window*)dwUser;

    if (m_nTimerID == window->play_timer_)
    {
        window->time_ += 0.001;

        if (window->time_ > window->max_time_)
        {
            window->time_ = 0;
        }

        InvalidateRect(window->hWnd, NULL, FALSE);
    }
}

void Window::OnPaint(HDC hdc)
{
    Graphics graphics(hdc);
}

void Window::SetMaxTime(double time)
{
    max_time_ = time;
}

void Window::Play()
{
    timeGetDevCaps(&timecaps, sizeof(TIMECAPS));

    timeKillEvent(play_timer_);
    time_ = 0;
    play_timer_ = timeSetEvent(1, timecaps.wPeriodMax, Window::GetInstance()->TimerProc, (DWORD_PTR)this, TIME_PERIODIC);
}

double Window::GetTime()
{
    return time_ * 1000;
}

void Window::UpdateWindowArea()
{
    GetClientRect(hWnd, &client_area_);
    window_area_ = { 0, 0, client_area_.right - client_area_.left, client_area_.bottom - client_area_.top };
}

Window* Window::GetInstance()
{
    call_once(flag_, [] // ���ٽ�
        {
            instance_.reset(new Window);
        });

    return instance_.get();
}

RECT Window::GetWindowArea()
{
    return window_area_;
}

Canvas* Window::GetCanvas()
{
    return canvas_.get();
}

FileManager* Window::GetFileManager()
{
    return file_manager_.get();
}
