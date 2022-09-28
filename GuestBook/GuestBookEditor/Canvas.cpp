#include "Canvas.h"
#include "Window.h"

Canvas::Canvas(HWND hWnd, int width, int height)
{
	this->hWnd = hWnd;
	width_ = width;
	height_ = height;
	UpdateWindowArea();
}

void Canvas::UpdateWindowArea()
{
	GetClientRect(hWnd, &client_area_);
	window_area_ = { 0, 0, client_area_.right - client_area_.left, client_area_.bottom - client_area_.top };
}

void Canvas::Reset()
{
	points_.clear();
}

void Canvas::MouseUp()
{
	if (is_canvas_click_)
	{
		is_canvas_click_ = false;
	}
}

void Canvas::MouseDown(POINT mouse_position)
{
	if (PtInRect(&canvas_area_, mouse_position))
	{
		is_canvas_click_ = true;
		current_x = mouse_position.x;
		current_y = mouse_position.y;
	}
}

void Canvas::MouseMove(POINT mouse_position, int width, double time, COLORREF color)
{
	if (is_canvas_click_)
	{
		if (!PtInRect(&canvas_area_, mouse_position))
		{
			MouseUp();
			return;
		}

		HDC hdc;
		hdc = GetDC(hWnd);
		HPEN n = CreatePen(PS_SOLID, width, color);
		HPEN o = (HPEN)SelectObject(hdc, n);
		MoveToEx(hdc, current_x, current_y, NULL);
		LineTo(hdc, mouse_position.x, mouse_position.y);
		points_.push_back({ current_x - x_, current_y - y_, mouse_position.x - x_, mouse_position.y - y_, width, time, color });
		SelectObject(hdc, o);
		DeleteObject(n);
		ReleaseDC(hWnd, hdc);
		current_x = mouse_position.x;
		current_y = mouse_position.y;
	}
}

void Canvas::Draw(HDC hdc)
{
	UpdateWindowArea();

	// 윈도우 크기에 따른 위치 보정
	x_ = (window_area_.right - width_ - 400) / 2;
	y_ = (window_area_.bottom - height_ - 300) / 2;

	Graphics graphics(hdc);

	// 배경 제거
	SetBkMode(hdc, TRANSPARENT);

	FontFamily arial_font(L"Arial");
	Font font_style(&arial_font, 12, FontStyleRegular, UnitPixel);

	Pen outline_pen(Color(255, 33, 35, 39));

	SolidBrush black_brush(Color(255, 0, 0, 0));
	SolidBrush background_brush(Color(255, 255, 255, 255));

	graphics.FillRectangle(&background_brush, x_, y_, width_, height_);
	graphics.DrawRectangle(&outline_pen, x_ - 1, y_ - 1, width_ + 1, height_ + 1);
	
	canvas_area_ = { x_, y_, x_ + width_, y_ + height_ };
}

void Canvas::DrawLine(HDC hdc, int idx)
{
	HPEN n = CreatePen(PS_SOLID, points_[idx].width, points_[idx].color);
	HPEN o = (HPEN)SelectObject(hdc, n);
	MoveToEx(hdc, points_[idx].start_x + x_, points_[idx].start_y + y_, NULL);
	LineTo(hdc, points_[idx].end_x + x_, points_[idx].end_y + y_);
	SelectObject(hdc, o);
	DeleteObject(n);
}

void Canvas::OpenSaveFile()
{
	WCHAR file_name[256] = L"";
	
	OPENFILENAME OFN;
	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = hWnd;
	OFN.lpstrFilter = L"Guest Book(*.gb)\0*.gb";
	OFN.lpstrDefExt = L"gb";
	OFN.lpstrFile = file_name;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = Window::GetInstance()->GetFileManager()->GetRootPath();
	OFN.Flags = OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

	size_t size = points_.size();

	if (GetSaveFileName(&OFN) != 0)
	{
		ofstream save(OFN.lpstrFile, ios::binary);
		if (save.is_open())
		{
			if (size != 0)
			{
				save.write((const char*)&size, 4);
				save.write((const char*)&points_[0], size * sizeof(PointInfo));
			}

			save.close();
		}
	}
}

void Canvas::OpenLoadFile()
{
	WCHAR file_name[256] = L"";
	WCHAR path[1024] = L"";

	OPENFILENAME OFN;
	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = hWnd;
	OFN.lpstrFilter = L"Guest Book(*.gb)\0*.gb";
	OFN.lpstrDefExt = L"gb";
	OFN.lpstrFile = file_name;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = Window::GetInstance()->GetFileManager()->GetRootPath();
	OFN.Flags = OFN_NOCHANGEDIR;

	if (GetOpenFileName(&OFN) != 0)
	{
		wsprintf(path, L"%s", OFN.lpstrFile);
		LoadGBFile(path);
		Window::GetInstance()->SetTimer(Window::GetInstance()->GetCanvas()->GetPoints()[Window::GetInstance()->GetCanvas()->GetPoints().size() - 1].time);
		Window::GetInstance()->GetTimeline()->UpdateMaxTime(Window::GetInstance()->GetCanvas()->GetPoints()[Window::GetInstance()->GetCanvas()->GetPoints().size() - 1].time);
	}
}

void Canvas::LoadGBFile(fs::path path)
{
	size_t size = 0;

	ifstream load(path, ios::binary);
	if (load.is_open())
	{
		load.read((CHAR*)&size, 4);
		points_.resize(size);
		load.read((char*)&points_[0], size * sizeof(PointInfo));
		load.close();
	}

	InvalidateRect(hWnd, &canvas_area_, FALSE);
}

bool Canvas::IsCanvasClick()
{
	return is_canvas_click_;
}

RECT* Canvas::GetCanvasArea()
{
	return &canvas_area_;
}

vector<Canvas::PointInfo> Canvas::GetPoints()
{
	return points_;
}