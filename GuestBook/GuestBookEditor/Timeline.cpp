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
}

void Timeline::MouseMove(POINT mouse_position)
{

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

	// 타임라인
	x_ = 0;
	y_ = 500;
	width_ = window_area_.right - 1;
	height_ = window_area_.bottom - y_ - 1;

	timeline_area_ = { x_, y_, width_, height_ };

	graphics.DrawRectangle(&red_pen, x_, y_, width_, height_ );

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

	graphics.FillPolygon(&black_brush, points, 5);
	graphics.DrawLine(&red_pen, Point(progress_x_ + (progress_ / max_progress_) * progress_width_, progress_y_ + progress_height_), Point(progress_x_ + (progress_ / max_progress_) * progress_width_, y_ + height_));
}