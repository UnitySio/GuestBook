#include "Window.h"

// ��� ���� �ʱ�ȭ
Window* Window::instance_ = nullptr;

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
        0, 0, 1280, 720, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

// �� â�� �޽����� ó��
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
        quick_panel = new QuickPanel(hWnd);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // �޴� ������ ���� �м��մϴ�:
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
    case WM_LBUTTONUP:
    {
        quick_panel->MouseUp();

        timeKillEvent(drawing_timer);
    }
    break;
    case WM_LBUTTONDOWN:
    {
        mouse_position.x = LOWORD(lParam);
        mouse_position.y = HIWORD(lParam);

        quick_panel->MouseDown(mouse_position);

        drawing_timer = timeSetEvent(1, timecaps.wPeriodMax, StaticTimerProc, (DWORD)this, TIME_PERIODIC);
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

void CALLBACK Window::StaticTimerProc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    instance_->timer += 0.001;
    InvalidateRect(instance_->hWnd, NULL, FALSE);
}

void Window::OnPaint(HDC hdc)
{
    Graphics graphics(hdc);

    SolidBrush black_brush(Color(255, 0, 0, 0));

    FontFamily arial_font(L"Arial");
    Font font_style(&arial_font, 12, FontStyleRegular, UnitPixel);

    WCHAR header_word[1024];
    _stprintf_s(header_word, L"Timer: %.3lfs", timer);

    PointF header_font_position(10, 10);
    graphics.DrawString(header_word, -1, &font_style, header_font_position, &black_brush);
}

Window::Window() : hInst(NULL), hWnd(NULL) // ��� ���� ����Ʈ �ʱ�ȭ
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

Window* Window::GetWindow()
{
    return instance_;
}
