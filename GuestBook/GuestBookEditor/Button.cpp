#include "Button.h"

Button::Button(function<void()> callback)
{
	callback_ = callback;

	is_interactable_ = true;

	background_color_ = Color(255, 238, 238, 238);
	contour_color_ = Color(255, 185, 185, 185);
	text_color_ = Color(255, 0, 0, 0);
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

void Button::Draw(HDC hdc, LPCWSTR text, int x, int y, int width, int height)
{
	Graphics graphics(hdc);

	SolidBrush black_brush(Color(255, 0, 0, 0));
	SolidBrush interactable_brush(Color(100, 255, 255, 255));
	SolidBrush background_brush(background_color_);
	SolidBrush text_brush(text_color_);

	Pen contour_pen(contour_color_);

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

	Region region(Rect(x_, y_, width_, height_));

	// 클리핑 마스크 시작
	graphics.SetClip(&region, CombineModeReplace);

	WCHAR text_word[1024];
	wsprintf(text_word, L"%s", text);
	PointF text_font_position(x_ + (width_ / 2), y_ + (height_ / 2));
	
	graphics.DrawString(text_word, -1, &font_style, text_font_position, &string_format, &text_brush);

	// 클리핑 마스크 종료
	graphics.ResetClip();

	// 비활성화 했을 경우
	if (!is_interactable_)
	{
		graphics.FillRectangle(&interactable_brush, x_, y_, width_, height_);
	}
}

void Button::SetInteractable(bool value)
{
	is_interactable_ = value;
}

void Button::SetBackgroundColor(Color color)
{
	background_color_ = color;
}

void Button::SetContourColor(Color color)
{
	contour_color_ = color;
}

void Button::SetTextColor(Color color)
{
	text_color_ = color;
}
