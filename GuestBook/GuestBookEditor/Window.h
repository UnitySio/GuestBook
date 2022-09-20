#pragma once

#include "resource.h"
#include "framework.h"
#include "QuickPanel.h"
#include "Timeline.h"
#include "Canvas.h"

#include <filesystem>
#include <fstream>

namespace fs = filesystem;

#define MAX_LOADSTRING 100

class Window
{
private:
	HINSTANCE hInst; // 현재 인스턴스
	HWND hWnd;

	static LRESULT CALLBACK StaticWndProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
	static void CALLBACK TimerProc(UINT m_nTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

	void OnPaint(HDC hdc);

	// 싱글톤
	static unique_ptr<Window> instance_;
	static once_flag flag_;

	RECT client_area_;
	RECT window_area_;

	UINT drawing_timer_;
	UINT play_timer_;

	double timer_;

	// 스마트 포인터(자동으로 메모리를 관리해 준다.)
	unique_ptr<Timeline> timeline_;
	unique_ptr<Canvas> canvas_;

	int current_x_;
	int current_y_;
public:
	Window() = default;
	~Window() = default;

	Window(const Window&) = delete;
	Window &operator=(const Window&) = delete;

	WCHAR szTitle[MAX_LOADSTRING]; // 제목 표시줄 텍스트입니다.
	WCHAR szWindowClass[MAX_LOADSTRING]; // 기본 창 클래스 이름입니다.

	ATOM MyRegisterClass(HINSTANCE hinstance);
	BOOL InitInstance(HINSTANCE, int);

	static Window* GetInstance();
};

