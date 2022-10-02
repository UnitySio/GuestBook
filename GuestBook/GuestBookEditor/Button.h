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

	WCHAR text_[1024];
	function<void()> callback_;

	Color background_color_;

	bool is_interactable_;
public:
	Button(LPCWSTR text, function<void()> callback);
	~Button() = default;

	void MouseDown(POINT mouse_position);
	void Draw(HDC hdc, int x, int y, int width, int height);

	void SetInteractable(bool value);
	void SetBackground(Color color);
};

