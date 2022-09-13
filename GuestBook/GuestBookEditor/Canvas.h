#pragma once

#include "framework.h"

#include <vector>

class Canvas
{
private:
	struct PointInfo
	{
		int start_x;
		int start_y;
		int end_x;
		int end_y;
		int width;

		double time;

		COLORREF color;
	};

	HWND hWnd;

	int x_;
	int y_;
	int width_;
	int height_;

	RECT canvas_area_;

	bool is_canvas_click_;

	vector<PointInfo> points_;

	int current_x;
	int current_y;
public:
	Canvas(HWND hWnd, int x, int y, int width, int height);
	~Canvas();

	void MouseUp();
	void MouseDown(POINT mouse_position);
	void MouseMove(POINT mouse_position, int width, double time, COLORREF color);
	void Draw(HDC hdc);

	vector<PointInfo> GetPoints();
};

