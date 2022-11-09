#include "pch.h"
#include "Control.h"
#include "Window.h"

using namespace std;
using namespace Gdiplus;

Control::Control(HWND hWnd)
{
	this->hWnd = hWnd;

	x_ = 0;
	y_ = 0;
	width_ = Window::GetInstance()->GetWindowArea().right;
	height_ = 60;

	button_color_ = make_unique<Button>([]
		{
			Window::GetInstance()->GetColorPicker()->Open();
		});

	button_play_ = make_unique<Button>([]
		{
			TIMECAPS timecaps;
			timeGetDevCaps(&timecaps, sizeof(TIMECAPS));

			Window::GetInstance()->GetTimeline()->Play();

			if (Window::GetInstance()->GetPlayTimer() == NULL)
			{
				Window::GetInstance()->SetPlayTimer(timeSetEvent(1, timecaps.wPeriodMax, Window::GetInstance()->TimerProc, (DWORD_PTR)Window::GetInstance(), TIME_PERIODIC));
			}
			else
			{
				timeKillEvent(Window::GetInstance()->GetPlayTimer());
				Window::GetInstance()->SetPlayTimer(NULL);
			}
		});
}

void Control::MouseDown(POINT mouse_position)
{
	button_color_->MouseDown(mouse_position);
	button_play_->MouseDown(mouse_position);
}

void Control::Draw(HDC hdc)
{
	Graphics graphics(hdc);

	SolidBrush background_brush(Color(255, 238, 238, 238));
	SolidBrush shadow_brush(Color(255, 132, 132, 132));

	SolidBrush black_brush(Color(255, 0, 0, 0));

	Pen contour_pen(Color(255, 185, 185, 185));

	width_ = Window::GetInstance()->GetWindowArea().right;

	graphics.FillRectangle(&background_brush, x_, y_, width_, height_);
	graphics.DrawRectangle(&contour_pen, x_, y_, width_ - 1, height_ - 1);

	BYTE r = Window::GetInstance()->GetColorPicker()->GetR();
	BYTE g = Window::GetInstance()->GetColorPicker()->GetG();
	BYTE b = Window::GetInstance()->GetColorPicker()->GetB();

	BYTE reversal_r = 255 -  Window::GetInstance()->GetColorPicker()->GetR();
	BYTE reversal_g = 255 - Window::GetInstance()->GetColorPicker()->GetG();
	BYTE reversal_b = 255 - Window::GetInstance()->GetColorPicker()->GetB();

	WCHAR pen_size_word[1024];
	wsprintf(pen_size_word, L"%d", Window::GetInstance()->GetColorPicker()->GetPenSize());
	button_color_->SetBackgroundColor(Color(255, r, g, b));
	button_color_->SetTextColor(Color(255, reversal_r, reversal_g, reversal_b));
	button_color_->Draw(hdc, pen_size_word, x_ + 5, y_ + 5, 50, 50);

	Image* play_image;
	if (Window::GetInstance()->GetTimeline()->OnPlay())
	{
		play_image = Image::FromFile(L"Resources/StopIcon.png");
		button_play_->SetBackgroundColor(Color(255, 219, 219, 219));
		button_play_->SetShadow(true);
	}
	else
	{
		play_image = Image::FromFile(L"Resources/PlayIcon.png");
		button_play_->SetBackgroundColor(Color(255, 238, 238, 238));
		button_play_->SetShadow(false);
	}

	button_play_->SetImage(play_image, 48, 48);
	button_play_->Draw(hdc, L"", x_ + ((Window::GetInstance()->GetWindowArea().right - 50) / 2), y_ + 5, 50, 50);
}

int Control::GetWidth()
{
	return width_;
}

int Control::GetHeight()
{
	return height_;
}
