#pragma once

#include "resource.h"
#include "framework.h"
#include "ColorPicker.h"
#include "Timeline.h"
#include "Canvas.h"
#include "FileManager.h"
#include "Control.h"
#include "Button.h"

#define MAX_LOADSTRING 100

class Window
{
private:
	HINSTANCE hInst; // ���� �ν��Ͻ�
	HWND hWnd;

	RECT client_area_;
	RECT window_area_;

	static LRESULT CALLBACK StaticWndProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

	void OnPaint(HDC hdc);

	// �̱���
	static unique_ptr<Window> instance_;
	static once_flag flag_;

	double timer_;

	// ����Ʈ ������(�ڵ����� �޸𸮸� ������ �ش�.)
	unique_ptr<Control> control_;
	unique_ptr<Timeline> timeline_;
	unique_ptr<FileManager> file_manager_;
	unique_ptr<Canvas> canvas_;
	unique_ptr<ColorPicker> color_picker_;

	UINT drawing_timer_;
	UINT play_timer_;

	int mouse_current_x_;
	int mouse_current_y_;

	void UpdateWindowArea();
public:
	Window() = default;
	~Window() = default;

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	WCHAR szTitle[MAX_LOADSTRING]; // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
	WCHAR szWindowClass[MAX_LOADSTRING]; // �⺻ â Ŭ���� �̸��Դϴ�.

	ATOM MyRegisterClass(HINSTANCE hinstance);
	BOOL InitInstance(HINSTANCE, int);

	static Window* GetInstance();

	HWND GetHWND();

	static void CALLBACK TimerProc(UINT m_nTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

	void SetTime(double time);

	RECT GetWindowArea();

	Control* GetControl();
	Timeline* GetTimeline();
	FileManager* GetFileManager();
	Canvas* GetCanvas();
	ColorPicker* GetColorPicker();

	void SetPlayTimer(UINT timer);
	UINT GetPlayTimer();
};

