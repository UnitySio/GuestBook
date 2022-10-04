#pragma once

#include "framework.h"
#include "Button.h"

class Control
{
private:
	HWND hWnd;

	int x_;
	int y_;
	int width_;
	int height_;

	unique_ptr<Button> undo_button_;
	unique_ptr<Button> redo_button_;
	unique_ptr<Button> color_button_;
	unique_ptr<Button> play_button_;
public:
	Control(HWND hWnd);
	~Control() = default;

	void MouseDown(POINT mouse_position);
	void Draw(HDC hdc);

	int GetWidth();
	int GetHeight();
};
