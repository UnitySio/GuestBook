#pragma once
#include "framework.h"

#include<functional>

class Button
{
private:
	int x_;
	int y_;
	int width_;
	int height_;

	int image_width_;
	int image_height_;

	RECT button_area_;

	function<void()> callback_;

	Color background_color_;
	Color contour_color_;
	Color shadow_color_;
	Color text_color_;

	bool is_interactable_;
	bool is_shadow_;
	bool is_background_;
	bool is_contour_;

	Image* image_;
public:
	Button(function<void()> callback);
	~Button() = default;

	void MouseDown(POINT mouse_position);
	void Draw(HDC hdc, LPCWSTR text, int x, int y, int width, int height);

	void SetInteractable(bool value);
	void SetShadow(bool value);
	void SetBackground(bool value);
	void SetContour(bool value);
	void SetBackgroundColor(Color color);
	void SetContourColor(Color color);
	void SetShadowColor(Color color);
	void SetTextColor(Color color);
	void SetImage(Image* image, int width, int height);
};

