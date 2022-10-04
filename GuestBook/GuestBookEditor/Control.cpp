#include "Control.h"
#include "Window.h"

Control::Control(HWND hWnd)
{
	this->hWnd = hWnd;

	x_ = 0;
	y_ = 0;
	width_ = Window::GetInstance()->GetWindowArea().right;
	height_ = 60;

	/*button_play_ = make_unique<Button>(L"", []
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
		});*/
}

void Control::MouseDown(POINT mouse_position)
{
	//button_play_->MouseDown(mouse_position);
}

void Control::Draw(HDC hdc)
{
	Graphics graphics(hdc);

	SolidBrush background_brush(Color(255, 238, 238, 238));
	SolidBrush black_brush(Color(255, 0, 0, 0));

	Pen contour_pen(Color(255, 185, 185, 185));

	width_ = Window::GetInstance()->GetWindowArea().right;

	graphics.FillRectangle(&background_brush, x_, y_, width_, height_);
	graphics.DrawRectangle(&contour_pen, x_, y_, width_ - 1, height_ - 1);

	/*button_play_->Draw(hdc, x_ + ((Window::GetInstance()->GetWindowArea().right - 50) / 2), 5, 50, 50, [](Graphics& graphics, int x, int y, int width, int height)
		{
			Image play_icon(L"Resources/PlayIcon.png");
			Image stop_icon(L"Resources/StopIcon.png");

			if (Window::GetInstance()->GetTimeline()->OnPlay())
			{
				graphics.DrawImage(&stop_icon, x + ((width - 48) / 2), y + ((height - 48) / 2), 48, 48);
			}
			else
			{
				graphics.DrawImage(&play_icon, x + ((width - 48) / 2), y + ((height - 48) / 2), 48, 48);
			}
		});*/
}

int Control::GetWidth()
{
	return width_;
}

int Control::GetHeight()
{
	return height_;
}
