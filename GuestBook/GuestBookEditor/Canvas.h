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

	vector<vector<PointInfo>> lines_;
	vector<PointInfo> line_;

	int mouse_current_x_;
	int mouse_current_y_;
public:
	Canvas(HWND hWnd);
	~Canvas() = default;

	void CanvasReset();

	void MouseUp();
	void MouseDown(POINT mouse_position);
	void MouseMove(POINT mouse_position, int width, double time, COLORREF color);
	void Draw(HDC hdc);
	void DrawLine(HDC hdc, size_t lines_idx, size_t line_idx);
	void OpenSaveFile();
	void OpenLoadFile();
	void LoadFile(fs::path path);

	bool OnCanvasClick();

	RECT GetCanvasArea();

	int GetWidth();
	int GetHeight();

	vector<vector<PointInfo>> GetLines();

	void Undo();
	void Redo();
};

