#include "Canvas.h"
#include "Window.h"

Canvas::Canvas(HWND hWnd)
{
	this->hWnd = hWnd;
	UpdateWindowArea();

	x_ = 0;
	y_ = Window::GetInstance()->GetControl()->GetHeight();
	width_ = window_area_.right - Window::GetInstance()->GetFileManager()->GetWidth();
	height_ = (window_area_.bottom - y_) - Window::GetInstance()->GetTimeline()->GetHeight();
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
		points_.push_back({ current_x - canvas_x_, current_y - canvas_y_, mouse_position.x - canvas_x_, mouse_position.y - canvas_y_, width, time, color });
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

	Graphics graphics(hdc);

	// ��� ����
	SetBkMode(hdc, TRANSPARENT);

	Image edit_icon(L"Resources/EditIcon.png");

	SolidBrush background_brush(Color(255, 219, 219, 219));
	SolidBrush background_brush2(Color(255, 238, 238, 238));
	SolidBrush white_brush(Color(255, 255, 255, 255));
	SolidBrush black_brush(Color(255, 0, 0, 0));

	Pen contour_pen(Color(255, 185, 185, 185));

	FontFamily arial_font(L"Arial");
	Font font_style(&arial_font, 12, FontStyleBold, UnitPixel);

	y_ = Window::GetInstance()->GetControl()->GetHeight();
	width_ = window_area_.right - Window::GetInstance()->GetFileManager()->GetWidth();
	height_ = (window_area_.bottom - y_) - Window::GetInstance()->GetTimeline()->GetHeight();

	graphics.FillRectangle(&background_brush, x_, y_, width_, height_);

	graphics.FillRectangle(&background_brush2, x_, y_ + 60, width_, height_ - 90);

	Region region(Rect(x_, y_ + 60, width_, height_ - 90));

	// Ŭ���� ����ũ ����
	graphics.SetClip(&region, CombineModeReplace);

	// ������ ũ�⿡ ���� ��ġ ����
	canvas_width_ = 1000;
	canvas_height_ = 500;
	canvas_x_ = ((x_ + width_) - canvas_width_) / 2;
	canvas_y_ = (((y_ + height_) + 90) - canvas_height_) / 2;

	graphics.FillRectangle(&white_brush, canvas_x_, canvas_y_, canvas_width_, canvas_height_);

	canvas_area_ = { canvas_x_, canvas_y_, canvas_x_ + canvas_width_, canvas_y_ + canvas_height_ };

	if (!Window::GetInstance()->GetTimeline()->OnPlay())
	{
		for (int i = 0; i < points_.size(); i++)
		{
			DrawLine(hdc, i);
		}
	}
	else
	{
		/*�Ʒ��� ���� ������� ����ؼ� �ٽ� �׸��� ����� ������ ������
		�������� Ž���� �ϸ鼭 ���ݾ� ���ݾ� �׸��� ��� ����� ��θ�,
		���� ����ϰ� �ִ� Ÿ�̸��� ������ ��ð��ȿ� ��� ������ �� ����
		������ �߻��ϱ� ������ �Ʒ��� ���� ����� ����Ͽ���.*/

		for (int i = 0; i < points_.size(); i++)
		{
			if ((int)trunc(points_[i].time * 1000) > Window::GetInstance()->GetTimeline()->GetTime())
			{
				break;
			}

			DrawLine(hdc, i);
		}
	}

	// Ŭ���� ����ũ ����
	graphics.ResetClip();

	/*���� GDI �׷��� ���̺귯�������� Ŭ������ ����� �� ���� ������
	�ٽ� �ѹ� �� �׸����� �ߴ�.*/
	graphics.FillRectangle(&background_brush, x_, y_, width_, 30);

	// ��ܹ�
	Point background_points[] = {
		Point(x_, y_),
		Point(x_, y_ + 60),
		Point(x_ + width_, y_ + 60),
		Point(x_ + width_, y_ + 30),
		Point(x_ + 90, y_ + 30),
		Point(x_ + 90, y_) };

	Point contour_points[] = {
		Point(x_, y_),
		Point(x_, y_ + 59),
		Point(x_ + width_ - 1, y_ + 59),
		Point(x_ + width_ - 1, y_ + 30),
		Point(x_ + 90, y_ + 30),
		Point(x_ + 90, y_) };

	graphics.FillPolygon(&background_brush2, background_points, 6);
	graphics.DrawPolygon(&contour_pen, contour_points, 6);

	graphics.DrawImage(&edit_icon, x_ + 10, y_ + 5, 20, 20);

	StringFormat string_format;
	string_format.SetLineAlignment(StringAlignmentCenter);

	PointF header_font_position(x_ + 35, y_ + 15);
	graphics.DrawString(L"ĵ����", -1, &font_style, header_font_position, &string_format, &black_brush);

	graphics.DrawRectangle(&contour_pen, x_, y_, width_ - 1, height_ - 1);

	// �ϴܹ�
	graphics.FillRectangle(&background_brush2, x_, y_ + height_ - 30, width_, 30);
	graphics.DrawRectangle(&contour_pen, x_, y_ + height_ - 30, width_ - 1, 29);

	WCHAR pen_size_word[1024];
	wsprintf(pen_size_word, L"�� ũ��: %d", Window::GetInstance()->GetQuickPanel()->GetPenSize());

	PointF pen_size_font_position(x_ + 5, y_ + height_ - 15);
	graphics.DrawString(pen_size_word, -1, &font_style, pen_size_font_position, &string_format, &black_brush);
}

void Canvas::DrawLine(HDC hdc, int idx)
{
	HPEN n = CreatePen(PS_SOLID, points_[idx].width, points_[idx].color);
	HPEN o = (HPEN)SelectObject(hdc, n);
	MoveToEx(hdc, points_[idx].start_x + canvas_x_, points_[idx].start_y + canvas_y_, NULL);
	LineTo(hdc, points_[idx].end_x + canvas_x_, points_[idx].end_y + canvas_y_);
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
		Window::GetInstance()->SetTimer(points_[points_.size() - 1].time);
		Window::GetInstance()->GetTimeline()->UpdateMaxTime(points_[points_.size() - 1].time);
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

bool Canvas::OnCanvasClick()
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

int Canvas::GetWidth()
{
	return width_;
}

int Canvas::GetHeight()
{
	return height_;
}