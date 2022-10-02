#include "Button.h"

Button::Button(LPCWSTR text, function<void()> callback)
{
	wsprintf(text_, L"%s", text);
	callback_ = callback;

	is_interactable_ = true;

	background_color_ = Color(255, 238, 238, 238);
}

void Button::MouseDown(POINT mouse_position)
{
	if (is_interactable_)
	{
		if (PtInRect(&button_area_, mouse_position))
		{
			callback_();
		}
	}
}

void Button::Draw(HDC hdc, int x, int y, int width, int height)
{
	Graphics graphics(hdc);

	SolidBrush black_brush(Color(255, 0, 0, 0));
	SolidBrush white100_brush(Color(100, 255, 255, 255));
	SolidBrush background_brush(background_color_);

	Pen contour_pen(Color(255, 185, 185, 185));

	FontFamily arial_font(L"Arial");
	Font font_style(&arial_font, 12, FontStyleBold, UnitPixel);

	StringFormat string_format;
	string_format.SetAlignment(StringAlignmentCenter);
	string_format.SetLineAlignment(StringAlignmentCenter);

	x_ = x;
	y_ = y;
	width_ = width;
	height_ = height;

	button_area_ = { x_, y_, x_ + width_, y_ + height_ };

	graphics.FillRectangle(&background_brush, x_, y_, width_, height_);
	graphics.DrawRectangle(&contour_pen, x_, y_, width_ - 1, height_ - 1);

	PointF text_font_position(x_ + (width_ / 2), y_ + (height_ / 2));


	Region region(Rect(x_, y_, width_, height_));

	// 클리핑 마스크 시작
	graphics.SetClip(&region, CombineModeReplace);

	graphics.DrawString(text_, -1, &font_style, text_font_position, &string_format, &black_brush);

	// 클리핑 마스크 종료
	graphics.ResetClip();

	if (!is_interactable_)
	{
		graphics.FillRectangle(&white100_brush, x_, y_, width_, height_);
	}
}

void Button::SetInteractable(bool value)
{
	is_interactable_ = value;
}

void Button::SetBackground(Color color)
{
	background_color_ = color;
}
