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

wchar_t* FileManager::ConvertBytes(uintmax_t bytes)
{
	const int byte = 1024;
	wchar_t result[256] = L"";
	string units[] = { "TB", "GB", "MB", "KB", "B" };
	wstring unit_to_wstring = L"";
	uintmax_t max = (uintmax_t)pow(byte, (sizeof(units) / sizeof(units[0])) - 1);

	for (string unit : units)
	{
		if (bytes > max)
		{
			unit_to_wstring.assign(unit.begin(), unit.end());
			wsprintf(result, L"%s%s", to_wstring(bytes / max).c_str(), unit_to_wstring.c_str());
			return result;
		}

		max /= byte;
	}

	return result;
}

void FileManager::MouseUp()
{
	if (is_list_box_click_ || is_scroll_bar_click_)
	{
		ReleaseCapture();
		is_list_box_click_ = false;
		is_item_click_ = false;
		is_scroll_bar_click_ = false;
	}
}

void FileManager::MouseDown(POINT mouse_position)
{
	if (PtInRect(&list_box_area_, mouse_position))
	{
		SetCapture(hWnd);
		
		for (int i = 0; i < list_box_items_.size(); i++)
		{
			RECT area = { list_box_x_, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * percent_) + (i * list_box_item_height_), list_box_x_ + list_box_width_, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * percent_) + (i * list_box_item_height_) + list_box_item_height_ };
			if (PtInRect(&area, mouse_position))
			{
				if (fs::is_directory(list_box_items_[i].file_path))
				{
					current_path_ = list_box_items_[i].file_path;
					item_select_ = 0;
					percent_ = 0;
					InvalidateRect(hWnd, &list_box_area_, FALSE);
				}
				else
				{
					// 해당 파일의 확장자가 .gb인지 확인
					if (fs::path(list_box_items_[i].file_path).extension() == ".gb")
					{
						is_item_click_ = true;
					}
				}
				break;
			}
		}

		is_list_box_click_ = true;
	}

	if (PtInRect(&scroll_bar_area_, mouse_position))
	{
		is_scroll_bar_click_ = true;
	}
}

void FileManager::MouseMove(POINT mouse_position)
{
	if (is_scroll_bar_click_)
	{
		if ((list_box_items_.size() * list_box_item_height_) > list_box_height_)
		{
			percent_ = min(max(((mouse_position.y - (scroll_bar_y_ + ((scroll_bar_height_ * ratio_) / 2))) * 1.0f) / (scroll_bar_height_ - (scroll_bar_height_ * ratio_)), 0), 1.0f);
			InvalidateRect(hWnd, NULL, FALSE);
		}
	}

	

	if (PtInRect(&list_box_area_, mouse_position))
	{
		for (int i = 0; i < list_box_items_.size(); i++)
		{
			RECT area = { list_box_x_, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * percent_) + (i * list_box_item_height_), list_box_x_ + list_box_width_, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * percent_) + (i * list_box_item_height_) + list_box_item_height_ };
			if (PtInRect(&area, mouse_position))
			{
				item_select_ = i;
				InvalidateRect(hWnd, NULL, FALSE);
				break;
			}
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
	x_ = window_area_.right - 500;
	y_ = window_area_.bottom - 300;
	width_ = window_area_.right - x_;
	height_ = window_area_.bottom - y_;

	graphics.FillRectangle(&background_brush, x_, y_, width_, height_);

	PointF header_font_position(x_ + 10, y_ + 9);
	graphics.DrawString(L"파일 매니저", -1, &font_style, header_font_position, &white_brush);

	// 스크롤바
	scroll_bar_width_ = 30;
	scroll_bar_height_ = 240;
	scroll_bar_x_ = x_ + 450;
	scroll_bar_y_ = (y_ + 30) + (((height_ - 30) - scroll_bar_height_) / 2);

	ratio_ = (double)scroll_bar_height_ / (list_box_items_.size() * list_box_item_height_);

	scroll_bar_area_ = { scroll_bar_x_, scroll_bar_y_, scroll_bar_x_ + scroll_bar_width_, scroll_bar_y_ + scroll_bar_height_ };

	graphics.FillRectangle(&white_brush, scroll_bar_x_, scroll_bar_y_, scroll_bar_width_, scroll_bar_height_);
	// Thumb
	if ((list_box_items_.size() * list_box_item_height_) > list_box_height_)
	{
		graphics.FillRectangle(&black_brush, scroll_bar_x_, scroll_bar_y_ + (percent_ / 1.0f) * (scroll_bar_height_ - (scroll_bar_height_ * ratio_)), scroll_bar_width_, scroll_bar_height_ * ratio_);
	}

	// 리스트 박스
	list_box_width_ = 440;
	list_box_height_ = 240;
	list_box_x_ = x_ + 10;
	list_box_y_ = (y_ + 30) + (((height_ - 30) - list_box_height_) / 2);

	list_box_area_ = { list_box_x_, list_box_y_, list_box_x_ + list_box_width_, list_box_y_ + list_box_height_ };

	graphics.FillRectangle(&white_brush, list_box_x_, list_box_y_, list_box_width_, list_box_height_);

	Region region(Rect(list_box_x_, list_box_y_, list_box_width_, list_box_height_));

	// 클리핑 마스크 시작
	graphics.SetClip(&region, CombineModeReplace);

	FileRefresh(current_path_);

	if (!list_box_items_.empty())
	{
		graphics.FillRectangle(&black_brush_, list_box_x_, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * percent_) + (item_select_ * list_box_item_height_), list_box_width_, list_box_item_height_);
	}

	Image file_icon(L"Resources/FileIcon.png");
	Image folder_icon(L"Resources/FolderIcon.png");

	WCHAR file_name_word[1024];
	WCHAR file_size_word[1024];
	StringFormat string_format;
	string_format.SetAlignment(StringAlignmentFar);

	for (int i = 0; i < list_box_items_.size(); i++)
	{
		PointF file_name_font_position(list_box_x_ + 35, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * percent_) + 17 + (i * list_box_item_height_));
		PointF file_size_font_position(list_box_x_ + list_box_width_ - 2.5, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * percent_) + 17 + (i * list_box_item_height_));
		wsprintf(file_name_word, L"%s", list_box_items_[i].file_name.c_str());
		wsprintf(file_size_word, L"%s", ConvertBytes(list_box_items_[i].file_size));

		
		if (fs::is_directory(list_box_items_[i].file_path))
		{
			graphics.DrawImage(&folder_icon, list_box_x_ + 2.5, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * percent_) + 10 + (i * list_box_item_height_), 30, 30);
		}
		else if (fs::is_regular_file(list_box_items_[i].file_path))
		{
			graphics.DrawImage(&file_icon, list_box_x_, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * percent_) + 10 + (i * list_box_item_height_), 30, 30);
		}

		graphics.DrawString(file_name_word, -1, &font_style, file_name_font_position, &black_brush);
		graphics.DrawString(file_size_word, -1, &font_style, file_size_font_position, &string_format, &black_brush);
	}

	// 클리핑 마스크 종료
	graphics.ResetClip();
}

void FileManager::FileRefresh(fs::path path)
{
	fs::path p(path);
	fs::directory_iterator iter(p);

	list_box_items_.clear();
	
	// 현재 폴더가 Geusts가 아닐 경우 상위 폴더와 최상위 폴더 표시
	if (p != "./Guests")
	{
		list_box_items_.push_back({ ".", "./Guests", NULL });
		list_box_items_.push_back({ "..", p.parent_path(), NULL });
	}

	while (iter != fs::end(iter))
	{
		const fs::directory_entry& entry = *iter;
		list_box_items_.push_back({ entry.path().filename(), entry.path(), entry.file_size()});
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
