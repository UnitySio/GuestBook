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

}

void FileManager::MouseDown(POINT mouse_position)
{
	
}

void FileManager::MouseMove(POINT mouse_position)
{
	for (int i = 0; i < listBoxItems_.size(); i++)
	{
		RECT area = { 0, i * 13, 300, (i * 13) + 13 };
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

	FontFamily arial_font(L"Arial");
	Font font_style(&arial_font, 12, FontStyleRegular, UnitPixel);

	SolidBrush white_brush(Color(255, 255, 255, 255));
	SolidBrush black_brush(Color(255, 0, 0, 0));
	SolidBrush black_brush_(Color(50, 0, 0, 0));

	Pen black_pen(Color(255, 0, 0, 0));

	Rect region_area = { 0, 0, 500, 100 };
	Region region(region_area);
	graphics.FillRectangle(&white_brush, 0, 0, 300, 100);
	graphics.DrawRectangle(&black_pen, 0, 0, 300, 100);
	
	// 클리핑 마스크 시작
	graphics.SetClip(&region, CombineModeReplace);

	FileRefresh();

	graphics.FillRectangle(&black_brush_, 0, item_select_ * 13, 300, 13);

	WCHAR word[1024];
	for (int i = 0; i < listBoxItems_.size(); i++)
	{
		PointF font_position(0, i * 13);
		wsprintf(word, L"%s", listBoxItems_[i].file_path.filename().c_str());
		graphics.DrawString(word, -1, &font_style, font_position, &black_brush);
	}

	// 클리핑 마스크 종료
	graphics.ResetClip();
}

void FileManager::FileRefresh()
{
	fs::path p("./Guests");
	fs::directory_iterator iter(p);

	listBoxItems_.clear();

	while (iter != fs::end(iter))
	{
		const fs::directory_entry& entry = *iter;
		listBoxItems_.push_back({ entry.path() });
		++iter;
	}
}

vector<FileManager::ListBoxItem> FileManager::GetListBoxItem()
{
	return listBoxItems_;
}

int FileManager::GetIndex()
{
	return item_select_;
}
