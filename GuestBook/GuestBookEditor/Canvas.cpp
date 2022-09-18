#include "Canvas.h"

Canvas::Canvas(HWND hWnd, int width, int height)
{
	this->hWnd = hWnd;
	width_ = width;
	height_ = height;
	UpdateWindowArea();
}

Canvas::~Canvas()
{

}

void Canvas::UpdateWindowArea()
{
	GetClientRect(hWnd, &client_area_);
	window_area_ = { 0, 0, client_area_.right - client_area_.left, client_area_.bottom - client_area_.top };
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
		points_.push_back({ current_x - x_, current_y, mouse_position.x - x_, mouse_position.y, width, time, color });
		SelectObject(hdc, o);
		DeleteObject(n);
		ReleaseDC(hWnd, hdc);
		current_x = mouse_position.x;
		current_y = mouse_position.y;
	}
}

void Canvas::Draw(HDC hdc)
{
	UpdateWindowArea();

	// 윈도우 크기에 따른 위치 보정
	x_ = (window_area_.right - width_) / 2;

	Graphics graphics(hdc);

	// 배경 제거
	SetBkMode(hdc, TRANSPARENT);

	FontFamily arial_font(L"Arial");
	Font font_style(&arial_font, 12, FontStyleRegular, UnitPixel);

	Pen outline_pen(Color(255, 33, 35, 39));

	SolidBrush black_brush(Color(255, 0, 0, 0));
	SolidBrush background_brush(Color(255, 255, 255, 255));

	graphics.FillRectangle(&background_brush, x_, y_, width_, height_);
	graphics.DrawRectangle(&outline_pen, x_ - 1, y_ - 1, width_ + 1, height_ + 1);
	
	canvas_area_ = { x_, y_, x_ + width_, y_ + height_ };
}

void Canvas::UpdateDraw(HDC hdc)
{
	for (int i = 0; i < points_.size(); i++)
	{
		HPEN n = CreatePen(PS_SOLID, points_[i].width, points_[i].color);
		HPEN o = (HPEN)SelectObject(hdc, n);
		MoveToEx(hdc, points_[i].start_x + x_, points_[i].start_y, NULL);
		LineTo(hdc, points_[i].end_x + x_, points_[i].end_y);
		SelectObject(hdc, o);
		DeleteObject(n);
	}
}

bool Canvas::IsCanvasClick()
{
	return is_canvas_click_;
}

int Canvas::GetX()
{
	return x_;
}

vector<Canvas::PointInfo> Canvas::GetPoints()
{
	return points_;
}