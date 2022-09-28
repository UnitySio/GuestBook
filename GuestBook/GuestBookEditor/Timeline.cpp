#include "Timeline.h"

Timeline::Timeline(HWND hWnd)
{
	this->hWnd = hWnd;
	UpdateWindowArea();
}

void Timeline::UpdateWindowArea()
{
	GetClientRect(hWnd, &client_area_);
	window_area_ = { 0, 0, client_area_.right - client_area_.left, client_area_.bottom - client_area_.top };
}

void Timeline::MouseUp()
{
	if (is_progress_click_)
	{
		ReleaseCapture();
		is_progress_click_ = false;
	}
}

void Timeline::MouseDown(POINT mouse_position)
{
	if (PtInRect(&timeline_area_, mouse_position))
	{
		SetCapture(hWnd);
		ProgressControl(mouse_position);
		is_progress_click_ = true;
	}
}

void Timeline::MouseMove(POINT mouse_position)
{
	if (!PtInRect(&window_area_, mouse_position))
	{
		MouseUp();
	}

	if (is_progress_click_)
	{
		ProgressControl(mouse_position);
	}
}

void Timeline::ProgressControl(POINT mouse_position)
{
	time_ = min(max(((mouse_position.x - progress_x_) * max_time_) / progress_width_, 0), max_time_);
	InvalidateRect(hWnd, NULL, FALSE);
}

void Timeline::AddTime(double time)
{
	time_ += time;

	if (time_ > max_time_)
	{
		time_ = 0;
		//InvalidateRect(hWnd, NULL, FALSE);
	}

	//InvalidateRect(hWnd, &timeline_area_, FALSE);
}

void Timeline::UpdateMaxTime(double time)
{
	max_time_ = time;
	InvalidateRect(hWnd, &timeline_area_, FALSE);
}

void Timeline::Draw(HDC hdc)
{
	UpdateWindowArea();

	Graphics graphics(hdc);

	// 배경 제거
	SetBkColor(hdc, TRANSPARENT);

	Pen black_pen(Color(255, 0, 0, 0));
	Pen white_pen(Color(255, 255, 255, 255));
	Pen yellow_pen(Color(255, 197, 193, 26));

	SolidBrush black_brush(Color(255, 0, 0, 0));
	SolidBrush white_brush(Color(255, 255, 255, 255));
	SolidBrush yellow_brush(Color(255, 197, 193, 26));
	SolidBrush background_brush(Color(255, 33, 35, 39));

	FontFamily arial_font(L"Arial");
	Font font_style(&arial_font, 12, FontStyleBold, UnitPixel);

	// 타임라인
	x_ = 0;
	y_ = window_area_.bottom - 300;
	width_ = window_area_.right;
	height_ = window_area_.bottom - y_;

	graphics.FillRectangle(&background_brush, x_, y_, width_, height_);

	WCHAR header_word[1024];
	_stprintf_s(header_word, L"타임라인 %.3lf초 / %.3lf초", time_, max_time_);

	PointF header_font_position(x_ + 10, y_ + 9);
	graphics.DrawString(header_word, -1, &font_style, header_font_position, &white_brush);

	timeline_area_ = { x_, y_, x_ + width_, y_ + height_ };

	// 프로그래스
	progress_x_ = x_;
	progress_y_ = y_ + 60;
	progress_width_ = width_;
	progress_height_ = 15;

	Point points[] = {
		Point(progress_x_ + (time_ / max_time_) * progress_width_ - 10, progress_y_),
		Point(progress_x_ + (time_ / max_time_) * progress_width_, progress_y_ + progress_height_),
		Point(progress_x_ + (time_ / max_time_) * progress_width_ + 10, progress_y_) };

	
	graphics.FillPolygon(&yellow_brush, points, 3);
	graphics.DrawLine(&yellow_pen, progress_x_ + (time_ / max_time_) * progress_width_, progress_y_ + progress_height_, progress_x_ + (time_ / max_time_) * progress_width_, y_ + height_);
}

void Timeline::Play()
{
	if (is_playing_)
	{
		time_ = 0;
	}

	is_playing_ = !is_playing_;

	InvalidateRect(hWnd, NULL, FALSE);
}

int Timeline::GetTime()
{
	return (int)trunc(time_ * 1000);
}

int Timeline::GetMaxTime()
{
	return (int)trunc(max_time_ * 1000);
}

int Timeline::GetWidth()
{
	return width_;
}

int Timeline::GetHeight()
{
	return height_;
}

int Timeline::GetX()
{
	return x_;
}

int Timeline::GetY()
{
	return y_;
}

bool Timeline::IsPlaying()
{
	return is_playing_;
}