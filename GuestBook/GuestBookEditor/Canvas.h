#pragma once

#include "framework.h"

#include <vector>
#include <filesystem>
#include <fstream>

namespace fs = filesystem;

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
	RECT client_area_;
	RECT window_area_;

	int x_;
	int y_;
	int width_;
	int height_;

	int canvas_x_;
	int canvas_y_;
	int canvas_width_;
	int canvas_height_;

	RECT canvas_area_;

	bool is_canvas_click_;

	vector<PointInfo> points_;

	int current_x;
	int current_y;

	void UpdateWindowArea();
public:
	Canvas(HWND hWnd);
	~Canvas() = default;

	void Reset();

	void MouseUp();
	void MouseDown(POINT mouse_position);
	void MouseMove(POINT mouse_position, int width, double time, COLORREF color);
	void Draw(HDC hdc);
	void DrawLine(HDC hdc, int idx);
	void OpenSaveFile();
	void OpenLoadFile();
	void LoadGBFile(fs::path path);

	bool OnCanvasClick();

	RECT* GetCanvasArea();
	vector<PointInfo> GetPoints();

	int GetWidth();
	int GetHeight();
};

