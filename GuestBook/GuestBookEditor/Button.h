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

	RECT button_area_;

	function<void()> callback_;

	Color background_color_;
	Color contour_color_;
	Color text_color_;

	bool is_interactable_;
public:
	Button(function<void()> callback);
	~Button() = default;

	void MouseDown(POINT mouse_position);
	void MouseMove(POINT mouse_position);
	void Draw(HDC hdc, LPCWSTR text, int x, int y, int width, int height);

	void SetInteractable(bool value);
	void SetBackgroundColor(Color color);
	void SetContourColor(Color color);
	void SetTextColor(Color color);
};

