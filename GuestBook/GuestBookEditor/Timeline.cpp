#include "Timeline.h"
#include "Window.h"

Timeline::Timeline(HWND hWnd)
{
	this->hWnd = hWnd;

	x_ = 0;
	height_ = 300;
	width_ = Window::GetInstance()->GetWindowArea().right;
	y_ = Window::GetInstance()->GetWindowArea().bottom - height_;
}

void Timeline::ScrollBarControl(POINT mouse_position)
{
	scroll_bar_thumb_percent_ = min(max(((mouse_position.y - (scroll_bar_y_ + (scroll_bar_thumb_height_ / 2))) * 1.0f) / (scroll_bar_height_ - scroll_bar_thumb_height_), 0), 1.0f);
	InvalidateRect(hWnd, &timeline_area_, FALSE);
}

void Timeline::MouseUp()
{
	if (is_list_box_click_ || is_scroll_bar_click_)
	{
		ReleaseCapture();
		is_list_box_click_ = false;
		is_scroll_bar_click_ = false;
	}
}

void Timeline::MouseDown(POINT mouse_position)
{
	if (PtInRect(&list_box_area_, mouse_position))
	{
		SetCapture(hWnd);
		KeyFrameControl(mouse_position);
		is_list_box_click_ = true;
	}

	if ((Window::GetInstance()->GetCanvas()->GetLines().size() * 30) > list_box_height_)
	{
		if (PtInRect(&scroll_bar_area_, mouse_position))
		{
			SetCapture(hWnd);
			ScrollBarControl(mouse_position);
			is_scroll_bar_click_ = true;
		}
	}
}

void Timeline::MouseMove(POINT mouse_position)
{
	RECT window_area = Window::GetInstance()->GetWindowArea();
	if (!PtInRect(&window_area, mouse_position))
	{
		MouseUp();
		return;
	}

	if (is_list_box_click_)
	{
		KeyFrameControl(mouse_position);
	}

	if (is_scroll_bar_click_)
	{
		ScrollBarControl(mouse_position);
	}
}

void Timeline::MouseWheel(POINT mouse_position, float direction)
{
	if (PtInRect(&list_box_area_, mouse_position))
	{
		if ((Window::GetInstance()->GetCanvas()->GetLines().size() * 30) > list_box_height_)
		{
			scroll_bar_thumb_percent_ = min(max(scroll_bar_thumb_percent_ - (direction / 10), 0), 1.0f);
			InvalidateRect(hWnd, &timeline_area_, FALSE);
		}
	}
}

void Timeline::KeyFrameControl(POINT mouse_position)
{
	time_ = min(max(((mouse_position.x - (list_box_x_ + 30)) * max_time_) / (list_box_width_ - 30), 0), max_time_);
	InvalidateRect(hWnd, NULL, FALSE);
}

void Timeline::AddTime(double time)
{
	time_ += time;

	if (time_ > max_time_)
	{
		time_ = 0;
	}

	InvalidateRect(hWnd, NULL, FALSE);
}

void Timeline::UpdateMaxTime(double time)
{
	max_time_ = time;
	InvalidateRect(hWnd, &timeline_area_, FALSE);
}

void Timeline::Draw(HDC hdc)
{
	Graphics graphics(hdc);

	// 배경 제거
	SetBkColor(hdc, TRANSPARENT);

	Image clock_icon(L"Resources/ClockIcon.png");

	Pen black_pen(Color(255, 0, 0, 0));
	Pen white_pen(Color(255, 255, 255, 255));
	Pen red_pen(Color(255, 255, 0, 0));
	Pen contour_pen(Color(255, 185, 185, 185));

	SolidBrush black_brush(Color(255, 0, 0, 0));
	SolidBrush red_brush(Color(255, 255, 0, 0));
	SolidBrush area_brush(Color(100, 255, 0, 0));
	SolidBrush background_brush(Color(255, 219, 219, 219));
	SolidBrush background_brush2(Color(255, 238, 238, 238));
	SolidBrush background_brush3(Color(255, 224, 224, 224));
	SolidBrush scroll_bar_brush(Color(255, 230, 230, 230));
	SolidBrush scroll_bar_thumb_brush(Color(255, 192, 192, 192));
	SolidBrush key_frame_brush(Color(255, 137, 147, 155));

	StringFormat string_format;
	string_format.SetLineAlignment(StringAlignmentCenter);

	FontFamily arial_font(L"Arial");
	Font font_style(&arial_font, 12, FontStyleBold, UnitPixel);

	// 타임라인
	width_ = Window::GetInstance()->GetWindowArea().right;
	y_ = Window::GetInstance()->GetWindowArea().bottom - height_;

	graphics.FillRectangle(&background_brush, x_, y_, width_, height_);

	Image minimize_icon(L"Resources/MinimizeIcon.png");
	graphics.DrawImage(&minimize_icon, x_ + width_ - 25, y_ + 5, 20, 20);

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
	graphics.DrawString(L"Timeline", -1, &font_style, header_font_position, &string_format, &black_brush);

	graphics.FillRectangle(&background_brush2, x_, y_ + 60, width_, height_ - 90);

	timeline_area_ = { x_, y_, x_ + width_, y_ + height_ };

	graphics.FillRectangle(&background_brush2, x_, y_ + 59, width_, 30);
	graphics.DrawRectangle(&contour_pen, x_, y_ + 59, width_ - 1, 29);

	StringFormat string_format_center;
	string_format_center.SetAlignment(StringAlignmentCenter);
	string_format_center.SetLineAlignment(StringAlignmentCenter);

	// 키 프레임
	list_box_x_ = x_;
	list_box_y_ = y_ + 90;
	list_box_width_ = width_ - 20;
	list_box_height_ = height_ - 120;

	list_box_area_ = { list_box_x_, list_box_y_, list_box_x_ + list_box_width_, list_box_y_ + list_box_height_ };

	Region region(Rect(list_box_x_, list_box_y_, list_box_width_, list_box_height_));

	// 클리핑 마스크 시작
	graphics.SetClip(&region, CombineModeReplace);

	Color key_frame_color(255, 255, 255, 255);
	SolidBrush key_frame_color_brush(key_frame_color);
	WCHAR number_word[1024];

	for (size_t i = 0; i < Window::GetInstance()->GetCanvas()->GetLines().size(); i++)
	{
		if ((i % 2) != 0)
		{
			graphics.FillRectangle(&background_brush3, list_box_x_, list_box_y_ - (((Window::GetInstance()->GetCanvas()->GetLines().size() * 30) - list_box_height_) * scroll_bar_thumb_percent_) + (i * 30), list_box_width_, 30);
		}
		
		key_frame_color.SetFromCOLORREF(Window::GetInstance()->GetCanvas()->GetLines()[i][0].color);
		key_frame_color_brush.SetColor(key_frame_color);

		graphics.FillRectangle(&background_brush2, list_box_x_ + 30 + (Window::GetInstance()->GetCanvas()->GetLines()[i][0].time / max_time_) * (list_box_width_ - 30), list_box_y_ - (((Window::GetInstance()->GetCanvas()->GetLines().size() * 30) - list_box_height_) * scroll_bar_thumb_percent_) + (i * 30), (Window::GetInstance()->GetCanvas()->GetLines()[i][Window::GetInstance()->GetCanvas()->GetLines()[i].size() - 1].time / max_time_) * (list_box_width_ - 30) - (Window::GetInstance()->GetCanvas()->GetLines()[i][0].time / max_time_) * (list_box_width_ - 30), 30);
		graphics.FillRectangle(&key_frame_color_brush, list_box_x_ + 30 + (Window::GetInstance()->GetCanvas()->GetLines()[i][0].time / max_time_) * (list_box_width_ - 30), list_box_y_ + 25 - (((Window::GetInstance()->GetCanvas()->GetLines().size() * 30) - list_box_height_) * scroll_bar_thumb_percent_) + (i * 30), (Window::GetInstance()->GetCanvas()->GetLines()[i][Window::GetInstance()->GetCanvas()->GetLines()[i].size() - 1].time / max_time_) * (list_box_width_ - 30) - (Window::GetInstance()->GetCanvas()->GetLines()[i][0].time / max_time_) * (list_box_width_ - 30), 5);
		graphics.DrawRectangle(&contour_pen, list_box_x_ + 30 + (Window::GetInstance()->GetCanvas()->GetLines()[i][0].time / max_time_) * (list_box_width_ - 30), list_box_y_ - (((Window::GetInstance()->GetCanvas()->GetLines().size() * 30) - list_box_height_) * scroll_bar_thumb_percent_) + (i * 30), (Window::GetInstance()->GetCanvas()->GetLines()[i][Window::GetInstance()->GetCanvas()->GetLines()[i].size() - 1].time / max_time_) * (list_box_width_ - 31) - (Window::GetInstance()->GetCanvas()->GetLines()[i][0].time / max_time_) * (list_box_width_ - 31), 29);

		graphics.FillRectangle(&background_brush2, list_box_x_, list_box_y_ - (((Window::GetInstance()->GetCanvas()->GetLines().size() * 30) - list_box_height_) * scroll_bar_thumb_percent_) + (i * 30), 30, 30);
		graphics.DrawRectangle(&contour_pen, list_box_x_, list_box_y_ - (((Window::GetInstance()->GetCanvas()->GetLines().size() * 30) - list_box_height_) * scroll_bar_thumb_percent_) + (i * 30), 29, 29);

		wsprintf(number_word, L"%d", ((int)i + 1));
		PointF number_font_position(list_box_x_ + 15, list_box_y_ - (((Window::GetInstance()->GetCanvas()->GetLines().size() * 30) - list_box_height_) * scroll_bar_thumb_percent_) + 15 + (i * 30));
		graphics.DrawString(number_word, -1, &font_style, number_font_position, &string_format_center, &black_brush);
	}

	graphics.DrawLine(&red_pen, list_box_x_ + 30 + (time_ / max_time_) * (list_box_width_ - 30), list_box_y_, list_box_x_ + 30 + (time_ / max_time_) * (list_box_width_ - 30), list_box_y_ + list_box_height_);
	
	// 클리핑 마스크 종료
	graphics.ResetClip();

	WCHAR word[1024];
	PointF key_frame_font_position(x_, y_ + 75);
	for (double i = 0; i < max_time_; i += 0.5f)
	{
		graphics.DrawLine(&black_pen, list_box_x_ + 30 + (i / max_time_) * (list_box_width_ - 30), y_ + 83, list_box_x_ + 30 + (i / max_time_) * (list_box_width_ - 30), y_ + 94);
		key_frame_font_position.X = list_box_x_ + 30 + (i / max_time_) * (list_box_width_ - 30);
		_stprintf_s(word, L"%.1lfs", i);
		graphics.DrawString(word, -1, &font_style, key_frame_font_position, &string_format_center, &black_brush);
	}


	Point header_points[] = {
		Point(x_ + 25 + (time_ / max_time_) * (width_ - 50), y_ + 80),
		Point(x_ + 30 + (time_ / max_time_) * (width_ - 50), y_ + 90),
		Point(x_ + 35 + (time_ / max_time_) * (width_ - 50), y_ + 80) };

	graphics.FillPolygon(&red_brush, header_points, 3);

	// 스크롤바
	scroll_bar_width_ = 20;
	scroll_bar_height_ = list_box_height_;
	scroll_bar_x_ = list_box_x_ + list_box_width_;
	scroll_bar_y_ = list_box_y_;

	scroll_bar_thumb_ratio_ = (double)scroll_bar_height_ / (Window::GetInstance()->GetCanvas()->GetLines().size() * 30);

	scroll_bar_area_ = { scroll_bar_x_, scroll_bar_y_, scroll_bar_x_ + scroll_bar_width_, scroll_bar_y_ + scroll_bar_height_ };

	graphics.FillRectangle(&scroll_bar_brush, scroll_bar_x_, scroll_bar_y_, scroll_bar_width_, scroll_bar_height_);

	// Scroll Bar Thumb
	if ((Window::GetInstance()->GetCanvas()->GetLines().size() * 30) > list_box_height_)
	{
		scroll_bar_thumb_height_ = scroll_bar_height_ * scroll_bar_thumb_ratio_;
		graphics.FillRectangle(&scroll_bar_thumb_brush, scroll_bar_x_ + 5, scroll_bar_y_ + 5 + (scroll_bar_thumb_percent_ / 1.0f) * (scroll_bar_height_ - scroll_bar_thumb_height_), scroll_bar_width_ - 10, round(scroll_bar_thumb_height_) - 10);
	}

	if ((Window::GetInstance()->GetCanvas()->GetLines().size() * 30) < list_box_height_ && scroll_bar_thumb_percent_ != 0)
	{
		scroll_bar_thumb_percent_ = 0;
	}

	graphics.DrawRectangle(&contour_pen, x_, y_, width_ - 1, height_ - 1);

	// 하단바
	graphics.FillRectangle(&background_brush2, x_, y_ + height_ - 30, width_, 30);
	graphics.DrawRectangle(&contour_pen, x_, y_ + height_ - 30, width_ - 1, 29);

	WCHAR timer_word[1024];
	_stprintf_s(timer_word, L"%.3lfs / %.3lfs", time_, max_time_);

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

RECT Timeline::GetTimelineArea()
{
	return timeline_area_;
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
