#include "Control.h"
#include "Window.h"

Control::Control(HWND hWnd)
{
	this->hWnd = hWnd;

	x_ = 0;
	y_ = 0;
	width_ = Window::GetInstance()->GetWindowArea().right;
	height_ = 60;
}

void Control::MouseDown(POINT mouse_position)
{
}

void Control::Draw(HDC hdc)
{
	Graphics graphics(hdc);

	SolidBrush background_brush(Color(255, 238, 238, 238));

	Pen contour_pen(Color(255, 185, 185, 185));

	width_ = Window::GetInstance()->GetWindowArea().right;

	graphics.FillRectangle(&background_brush, x_, y_, width_, height_);
	graphics.DrawRectangle(&contour_pen, x_, y_, width_ - 1, height_ - 1);
}

int Control::GetWidth()
{
	return width_;
}

int Control::GetHeight()
{
	return height_;
}
