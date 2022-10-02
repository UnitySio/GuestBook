#pragma once

#include "framework.h"
#include "Button.h"

class Control
{
private:
	HWND hWnd;
	RECT client_area_;
	RECT window_area_;

	int x_;
	int y_;
	int width_;
	int height_;

	void UpdateWindowArea();
public:
	Control(HWND hWnd);
	~Control() = default;

	void MouseDown(POINT mouse_position);
	void Draw(HDC hdc);

	int GetWidth();
	int GetHeight();
};
