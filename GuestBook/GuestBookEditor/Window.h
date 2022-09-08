#pragma once

#include "resource.h"
#include "framework.h"
#include "QuickPanel.h"
#include "Timeline.h"

#define MAX_LOADSTRING 100

class Window
{
private:
	HINSTANCE hInst; // 현재 인스턴스
	HWND hWnd;

	static LRESULT CALLBACK StaticWndProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
	static void CALLBACK StaticTimerProc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

	void OnPaint(HDC hdc);

	// 싱글톤
	static Window* instance_;

	UINT drawing_timer;
	double timer;
public:
	Window();
	~Window();

	WCHAR szTitle[MAX_LOADSTRING]; // 제목 표시줄 텍스트입니다.
	WCHAR szWindowClass[MAX_LOADSTRING]; // 기본 창 클래스 이름입니다.

	ATOM MyRegisterClass(HINSTANCE hinstance);
	BOOL InitInstance(HINSTANCE, int);

	static void Create();
	static Window* GetWindow();
};

