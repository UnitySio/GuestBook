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

void Canvas::MouseMove(POINT mouse_position, int width, double time, COLORREF color)
{
	if (is_canvas_click_)
	{
		if (!PtInRect(&canvas_area_, mouse_position))
		{
			MouseUp();
			return;
		}

		HDC hdc;
		hdc = GetDC(hWnd);
		HPEN n = CreatePen(PS_SOLID, width, color);
		HPEN o = (HPEN)SelectObject(hdc, n);
		MoveToEx(hdc, current_x, current_y, NULL);
		LineTo(hdc, mouse_position.x, mouse_position.y);
		points_.push_back({ current_x, current_y, mouse_position.x, mouse_position.y, width, time, color });
		SelectObject(hdc, o);
		DeleteObject(n);
		ReleaseDC(hWnd, hdc);
		current_x = mouse_position.x;
		current_y = mouse_position.y;
	}
}

void Canvas::Draw(HDC hdc)
{
	Graphics graphics(hdc);

	FontFamily arial_font(L"Arial");
	Font font_style(&arial_font, 12, FontStyleRegular, UnitPixel);

	Pen black_pen(Color(255, 0, 0, 0));
	graphics.DrawRectangle(&black_pen, x_, y_, width_, height_);

	SolidBrush black_brush(Color(255, 0, 0, 0));

	PointF header_font_position(x_ + 10, y_ - 18);
	graphics.DrawString(L"Äµ¹ö½º", -1, &font_style, header_font_position, &black_brush);

	canvas_area_ = { x_, y_, x_ + width_, y_ + height_ };
}

vector<Canvas::PointInfo> Canvas::GetPoints()
{
	return points_;
}