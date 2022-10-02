#include "Timeline.h"

Timeline::Timeline(HWND hWnd)
{
	this->hWnd = hWnd;
	UpdateWindowArea();

	x_ = 0;
	height_ = 300;
	width_ = window_area_.right;
	y_ = window_area_.bottom - height_;
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
		return;
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

	Image clock_icon(L"Resources/ClockIcon.png");

	Pen black_pen(Color(255, 0, 0, 0));
	Pen white_pen(Color(255, 255, 255, 255));
	Pen red_pen(Color(255, 255, 0, 0));
	Pen contour_pen(Color(255, 185, 185, 185));

	SolidBrush black_brush(Color(255, 0, 0, 0));
	SolidBrush yellow_brush(Color(255, 197, 193, 26));
	SolidBrush background_brush(Color(255, 219, 219, 219));
	SolidBrush background_brush2(Color(255, 238, 238, 238));

	StringFormat string_format;
	string_format.SetLineAlignment(StringAlignmentCenter);

	FontFamily arial_font(L"Arial");
	Font font_style(&arial_font, 12, FontStyleBold, UnitPixel);

	// 타임라인
	width_ = window_area_.right;
	y_ = window_area_.bottom - height_;

	graphics.FillRectangle(&background_brush, x_, y_, width_, height_);

	// 상단바
	Point background_points[] = {
		Point(x_, y_),
		Point(x_, y_ + 60),
		Point(x_ + width_, y_ + 60),
		Point(x_ + width_, y_ + 30),
		Point(x_ + 105, y_ + 30),
		Point(x_ + 105, y_) };

	Point contour_points[] = {
		Point(x_, y_),
		Point(x_, y_ + 59),
		Point(x_ + width_ - 1, y_ + 59),
		Point(x_ + width_ - 1, y_ + 30),
		Point(x_ + 105, y_ + 30),
		Point(x_ + 105, y_) };

	graphics.FillPolygon(&background_brush2, background_points, 6);
	graphics.DrawPolygon(&contour_pen, contour_points, 6);

	graphics.DrawImage(&clock_icon, x_ + 10, y_ + 5, 20, 20);

	PointF header_font_position(x_ + 35, y_ + 15);
	graphics.DrawString(L"타임라인", -1, &font_style, header_font_position, &string_format, &black_brush);

	graphics.FillRectangle(&background_brush2, x_, y_ + 60, width_, height_ - 90);

	timeline_area_ = { x_, y_, x_ + width_, y_ + height_ };

	// 프로그래스
	progress_x_ = x_;
	progress_y_ = y_ + 45;
	progress_width_ = width_;
	progress_height_ = 15;

	graphics.DrawLine(&red_pen, progress_x_ + (time_ / max_time_) * progress_width_, progress_y_ + progress_height_, progress_x_ + (time_ / max_time_) * progress_width_, y_ + height_);

	graphics.DrawRectangle(&contour_pen, x_, y_, width_ - 1, height_ - 1);

	// 하단바
	graphics.FillRectangle(&background_brush2, x_, y_ + height_ - 30, width_, 30);
	graphics.DrawRectangle(&contour_pen, x_, y_ + height_ - 30, width_ - 1, 29);

	WCHAR timer_word[1024];
	_stprintf_s(timer_word, L"%.3lf초 / %.3lf초", time_, max_time_);

	PointF timer_font_position(x_ + 5, y_ + height_ - 15);
	graphics.DrawString(timer_word, -1, &font_style, timer_font_position, &string_format, &black_brush);
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

int Timeline::GetWidth()
{
	return width_;
}

int Timeline::GetHeight()
{
	return height_;
}

int Timeline::GetTime()
{
	return (int)trunc(time_ * 1000);
}

int Timeline::GetMaxTime()
{
	return (int)trunc(max_time_ * 1000);
}

bool Timeline::OnPlay()
{
	return is_playing_;
}
