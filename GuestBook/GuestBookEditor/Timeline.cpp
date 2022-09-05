#include "Timeline.h"

Timeline::Timeline(HWND hWnd)
{
	this->hWnd = hWnd;
	UpdateWindowArea();
}

Timeline::~Timeline()
{

}

void Timeline::UpdateWindowArea()
{
	GetClientRect(hWnd, &client_area_);
	window_area_ = { 0, 0, client_area_.right - client_area_.left, client_area_.bottom - client_area_.top };
}

void Timeline::MouseUp()
{
	ReleaseCapture();
	is_progress_click = false;
}

void Timeline::MouseDown(POINT mouse_position)
{
	SetCapture(hWnd);
	if (PtInRect(&timeline_area_, mouse_position))
	{
		ProgressControl(mouse_position);
		is_progress_click = true;
	}
}

void Timeline::MouseMove(POINT mouse_position)
{
	if (PtInRect(&window_area_, mouse_position) == false)
	{
		MouseUp();
	}
	if (is_progress_click)
	{
		ProgressControl(mouse_position);
	}
}

void Timeline::ProgressControl(POINT mouse_position)
{
	progress_ = min(max(((mouse_position.x - progress_x_) * max_progress_) / progress_width_, 0), max_progress_);
	InvalidateRect(hWnd, NULL, FALSE);
}

void Timeline::Draw(HDC hdc)
{
	UpdateWindowArea();

	Graphics graphics(hdc);

	// 배경 제거
	SetBkColor(hdc, TRANSPARENT);

	Pen black_pen(Color(255, 0, 0, 0));
	Pen red_pen(Color(255, 255, 0, 0));

	SolidBrush black_brush(Color(255, 0, 0, 0));
	SolidBrush white_brush(Color(255, 255, 255, 255));
	SolidBrush background_brush(Color(255, 33, 35, 39));

	FontFamily arial_font(L"Arial");
	Font font_style(&arial_font, 12, FontStyleRegular, UnitPixel);

	// 타임라인
	x_ = 0;
	y_ = window_area_.bottom - 200;
	width_ = window_area_.right;
	height_ = window_area_.bottom - y_;

	WCHAR header_word[1024];
	_stprintf_s(header_word, L"타임라인 %.3lf초 / %.3lf초", progress_, max_progress_);

	PointF header_font_position(x_, y_ - 15);
	graphics.DrawString(header_word, -1, &font_style, header_font_position, &black_brush);

	timeline_area_ = { x_, y_, x_ + width_, y_ + height_ };

	graphics.FillRectangle(&background_brush, x_, y_, width_, height_);

	// 프로그래스
	progress_x_ = x_;
	progress_y_ = y_;
	progress_width_ = width_;
	progress_height_ = 15;

	Point points[] = {
		Point(progress_x_ + (progress_ / max_progress_) * progress_width_ - 5, progress_y_),
		Point(progress_x_ + (progress_ / max_progress_) * progress_width_ - 5, progress_y_ + 10),
		Point(progress_x_ + (progress_ / max_progress_) * progress_width_, progress_y_ + progress_height_),
		Point(progress_x_ + (progress_ / max_progress_) * progress_width_ + 5, progress_y_ + 10),
		Point(progress_x_ + (progress_ / max_progress_) * progress_width_ + 5, progress_y_), };

	
	graphics.FillPolygon(&white_brush, points, 5);
	graphics.DrawLine(&red_pen, Point(progress_x_ + (progress_ / max_progress_) * progress_width_, progress_y_ + progress_height_), Point(progress_x_ + (progress_ / max_progress_) * progress_width_, y_ + height_));
}