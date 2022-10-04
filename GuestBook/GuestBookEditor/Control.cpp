#include "Control.h"
#include "Window.h"

Control::Control(HWND hWnd)
{
	this->hWnd = hWnd;

	x_ = 0;
	y_ = 0;
	width_ = Window::GetInstance()->GetWindowArea().right;
	height_ = 60;

	undo_button_ = make_unique<Button>(L"", [=]
		{
			MessageBox(hWnd, L"Undo", L"Notification", MB_OK);
		});

	redo_button_ = make_unique<Button>(L"", [=]
		{
			MessageBox(hWnd, L"Redo", L"Notification", MB_OK);
		});

	color_button_ = make_unique<Button>(L"0", [=]
		{
			if (!Window::GetInstance()->GetTimeline()->OnPlay())
			{
				Window::GetInstance()->GetColorPicker()->Open();
			}
		});

	play_button_ = make_unique<Button>(L"", [=]
		{
			MessageBox(hWnd, L"Play", L"Notification", MB_OK);
		});
}

void Control::MouseDown(POINT mouse_position)
{
	undo_button_->MouseDown(mouse_position);
	redo_button_->MouseDown(mouse_position);
	color_button_->MouseDown(mouse_position);
	play_button_->MouseDown(mouse_position);
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

	undo_button_->Draw(hdc, 5, 5, 50, 50, [](Graphics& graphics, int x, int y, int width, int height)
		{
			Image image(L"Resources/UndoIcon.png");
			graphics.DrawImage(&image, x + ((width - 30) / 2), y + ((height - 30) / 2), 30, 30);
		});

	redo_button_->Draw(hdc, 60, 5, 50, 50, [](Graphics& graphics, int x, int y, int width, int height)
		{
			Image image(L"Resources/RedoIcon.png");
			graphics.DrawImage(&image, x + ((width - 30) / 2), y + ((height - 30) / 2), 30, 30);
		});

	color_button_->Draw(hdc, 115, 5, 50, 50, [](Graphics& graphics, int x, int y, int width, int height)
		{
			Color color;
			color.SetFromCOLORREF(Window::GetInstance()->GetColorPicker()->GetRGB());

			SolidBrush color_brush(color);

			graphics.FillRectangle(&color_brush, x + ((width - 30) / 2), y + ((width - 30) / 2), 30, 30);
		});

	WCHAR pen_size_word[1024];
	wsprintf(pen_size_word, L"%d", Window::GetInstance()->GetColorPicker()->GetPenSize());
	color_button_->SetText(pen_size_word);

	play_button_->Draw(hdc, (Window::GetInstance()->GetWindowArea().right - 50) / 2, 5, 50, 50, [](Graphics& graphics, int x, int y, int width, int height)
		{
			Image image(L"Resources/PlayIcon.png");
			graphics.DrawImage(&image, x + ((width - 48) / 2), y + ((height - 48) / 2), 48, 48);
		});
}

int Control::GetWidth()
{
	return width_;
}

int Control::GetHeight()
{
	return height_;
}
