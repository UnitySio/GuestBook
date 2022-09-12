#include "Canvas.h"

Canvas::Canvas(HWND hWnd, int x, int y, int width, int height)
{
	this->hWnd = hWnd;
	x_ = x;
	y_ = y;
	width_ = width;
	height_ = height;
}

Canvas::~Canvas()
{

}

void Canvas::MouseUp()
{
	is_canvas_click_ = false;
}

void Canvas::MouseDown(POINT mouse_position)
{
	if (PtInRect(&canvas_area_, mouse_position))
	{
		is_canvas_click_ = true;
		current_x = mouse_position.x;
		current_y = mouse_position.y;
	}
}

void Canvas::MouseMove(POINT mouse_position)
{
	if (is_canvas_click_)
	{
		if (!PtInRect(&canvas_area_, mouse_position))
		{
			current_x = mouse_position.x;
			current_y = mouse_position.y;
			return;
		}

		HDC hdc;
		hdc = GetDC(hWnd);
		COLORREF color = RGB(255, 0, 0);
		HPEN n = CreatePen(PS_SOLID, 10, color);
		HPEN o = (HPEN)SelectObject(hdc, n);
		MoveToEx(hdc, current_x, current_y, NULL);
		LineTo(hdc, mouse_position.x, mouse_position.y);
		SelectObject(hdc, o);
		DeleteObject(n);
		ReleaseDC(hWnd, hdc);
		current_x = mouse_position.x;
		current_y = mouse_position.y;
	}
}

bool Canvas::IsInCanvas(POINT mouse_position)
{
	if (PtInRect(&canvas_area_, mouse_position))
	{
		return true;
	}

	return false;
}

void Canvas::Draw(HDC hdc)
{
	Graphics graphics(hdc);

	Pen black_pen(Color(255, 0, 0, 0));
	graphics.DrawRectangle(&black_pen, x_, y_, width_, height_);

	canvas_area_ = { x_, y_, x_ + width_, y_ + height_ };

}