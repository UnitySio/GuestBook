#include "FileManager.h"

FileManager::FileManager(HWND hWnd)
{
	this->hWnd = hWnd;
	UpdateWindowArea();
}

void FileManager::UpdateWindowArea()
{
	GetClientRect(hWnd, &client_area_);
	window_area_ = { 0, 0, client_area_.right - client_area_.left, client_area_.bottom - client_area_.top };
}

void FileManager::MouseUp()
{
	is_item_click_ = false;
}

void FileManager::MouseDown(POINT mouse_position)
{
	for (int i = 0; i < list_box_items_.size(); i++)
	{
		RECT area = { list_box_x_, list_box_y_ + (i * 30), list_box_x_ + list_box_width_, list_box_y_ + (i * 30) + 30 };
		if (PtInRect(&area, mouse_position))
		{
			is_item_click_ = true;
			break;
		}
	}
}

void FileManager::MouseMove(POINT mouse_position)
{
	for (int i = 0; i < list_box_items_.size(); i++)
	{
		RECT area = { list_box_x_, list_box_y_ + (i * 30), list_box_x_ + list_box_width_, list_box_y_ + (i * 30) + 30 };
		if (PtInRect(&area, mouse_position))
		{
			item_select_ = i;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
	}
}

void FileManager::Draw(HDC hdc)
{
	UpdateWindowArea();

	Graphics graphics(hdc);

	SolidBrush white_brush(Color(255, 255, 255, 255));
	SolidBrush black_brush(Color(255, 0, 0, 0));
	SolidBrush black_brush_(Color(50, 0, 0, 0));
	SolidBrush background_brush(Color(255, 33, 35, 39));

	Pen black_pen(Color(255, 0, 0, 0));

	FontFamily arial_font(L"Arial");
	Font font_style(&arial_font, 12, FontStyleBold, UnitPixel);

	// 파일 매니저
	x_ = window_area_.right - 300;
	y_ = window_area_.bottom - 300;
	width_ = window_area_.right - x_;
	height_ = window_area_.bottom - y_;

	graphics.FillRectangle(&background_brush, x_, y_, width_, height_);

	PointF header_font_position(x_ + 10, y_ + 9);
	graphics.DrawString(L"파일 관리자", -1, &font_style, header_font_position, &white_brush);

	// 리스트 박스
	list_box_width_ = 260;
	list_box_height_ = 240;
	list_box_x_ = x_ + ((width_ - list_box_width_) / 2);
	list_box_y_ = (y_ + 30) + (((height_ - 30) - list_box_height_) / 2);

	graphics.FillRectangle(&white_brush, list_box_x_, list_box_y_, list_box_width_, list_box_height_);

	Region region(Rect(list_box_x_, list_box_y_, list_box_width_, list_box_height_));

	// 클리핑 마스크 시작
	graphics.SetClip(&region, CombineModeReplace);

	FileRefresh();

	graphics.FillRectangle(&black_brush_, list_box_x_, list_box_y_ + (item_select_ * 30), list_box_width_, 30);
	
	Image file_image(L"Resources/File.png");

	WCHAR file_name_word[1024];
	WCHAR file_size_word[1024];
	StringFormat string_format;
	string_format.SetAlignment(StringAlignmentFar);
	for (int i = 0; i < list_box_items_.size(); i++)
	{
		PointF file_name_font_position(list_box_x_ + 25, list_box_y_ + 8 + (i * 30));
		PointF file_size_font_position(list_box_x_ + list_box_width_, list_box_y_ + 8 + (i * 30));
		wsprintf(file_name_word, L"%s", list_box_items_[i].file_path.filename().c_str());
		wsprintf(file_size_word, L"%dKB", fs::file_size(list_box_items_[i].file_path) / 1000);
		graphics.DrawImage(&file_image, list_box_x_, list_box_y_ + 2.5 + (i * 30), 25, 25);
		graphics.DrawString(file_name_word, -1, &font_style, file_name_font_position, &black_brush);
		graphics.DrawString(file_size_word, -1, &font_style, file_size_font_position, &string_format, &black_brush);
	}

	// 클리핑 마스크 종료
	graphics.ResetClip();
}

void FileManager::FileRefresh()
{
	fs::path p("./Guests");
	fs::directory_iterator iter(p);

	list_box_items_.clear();

	while (iter != fs::end(iter))
	{
		const fs::directory_entry& entry = *iter;
		list_box_items_.push_back({ entry.path() });
		++iter;
	}
}

vector<FileManager::ListBoxItem> FileManager::GetListBoxItem()
{
	return list_box_items_;
}

int FileManager::GetIndex()
{
	return item_select_;
}

bool FileManager::IsItemClick()
{
	return is_item_click_;
}
