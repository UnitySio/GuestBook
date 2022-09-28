#include "FileManager.h"
#include "Window.h"

FileManager::FileManager(HWND hWnd)
{
	this->hWnd = hWnd;
	UpdateWindowArea();
	wsprintf(current_path_, L"%s\\Guests", fs::current_path().c_str());
}

void FileManager::UpdateWindowArea()
{
	GetClientRect(hWnd, &client_area_);
	window_area_ = { 0, 0, client_area_.right - client_area_.left, client_area_.bottom - client_area_.top };
}

void FileManager::ScrollBarControl(POINT mouse_position)
{
	scroll_bar_thumb_percent_ = min(max(((mouse_position.y - (scroll_bar_y_ + (scroll_bar_thumb_height_ / 2))) * 1.0f) / (scroll_bar_height_ - scroll_bar_thumb_height_), 0), 1.0f);
	InvalidateRect(hWnd, &file_manager_area_, FALSE);
}

wchar_t* FileManager::ConvertBytes(uintmax_t bytes)
{
	const int byte = 1024;
	wchar_t result[256] = L"0B";
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
	if (is_scroll_bar_click_)
	{
		ReleaseCapture();
		is_scroll_bar_click_ = false;
	}
}

void FileManager::MouseDown(POINT mouse_position)
{
	if (PtInRect(&list_box_area_, mouse_position))
	{
		for (int i = 0; i < list_box_items_.size(); i++)
		{
			RECT item_area = { list_box_x_, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * scroll_bar_thumb_percent_) + (i * list_box_item_height_), list_box_x_ + list_box_width_ - 35, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * scroll_bar_thumb_percent_) + (i * list_box_item_height_) + list_box_item_height_ };
			if (PtInRect(&item_area, mouse_position))
			{
				SetCapture(hWnd);
				if (fs::is_directory(list_box_items_[i].file_path))
				{
					wsprintf(current_path_, L"%s", list_box_items_[i].file_path.c_str());
					list_item_select_ = 0;
					scroll_bar_thumb_percent_ = 0;
					InvalidateRect(hWnd, &file_manager_area_, FALSE);
				}
				else if (fs::is_regular_file(list_box_items_[i].file_path))
				{
					// 해당 파일의 확장자가 .gb인지 확인
					if (fs::path(list_box_items_[i].file_path).extension() == ".gb")
					{
						Window::GetInstance()->canvas_->LoadGBFile(list_box_items_[list_item_select_].file_path.string());
						Window::GetInstance()->timer_ = Window::GetInstance()->canvas_->GetPoints()[Window::GetInstance()->canvas_->GetPoints().size() - 1].time;
						Window::GetInstance()->timeline_->UpdateMaxTime(Window::GetInstance()->canvas_->GetPoints()[Window::GetInstance()->canvas_->GetPoints().size() - 1].time);
					}
				}
				break;
			}

			WCHAR file_name_word[1024];
			wsprintf(file_name_word, L"%s을(를) 삭제하시겠습니까?", list_box_items_[i].file_name.c_str());
			RECT trash_item_area = { list_box_x_ + list_box_width_ - 35, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * scroll_bar_thumb_percent_) + (i * list_box_item_height_), list_box_x_ + list_box_width_, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * scroll_bar_thumb_percent_) + (i * list_box_item_height_) + list_box_item_height_ };
			if (PtInRect(&trash_item_area, mouse_position))
			{
				if (MessageBox(hWnd, file_name_word, L"", MB_YESNO) == IDYES)
				{
					if (fs::is_directory(list_box_items_[i].file_path))
					{
						fs::remove_all(list_box_items_[i].file_path);
					}
					else if (fs::is_regular_file(list_box_items_[i].file_path))
					{
						fs::remove(list_box_items_[i].file_path);
					}

					InvalidateRect(hWnd, &file_manager_area_, FALSE);
				}
			}
		}
	}

	if ((list_box_items_.size() * list_box_item_height_) > list_box_height_)
	{
		if (PtInRect(&scroll_bar_area_, mouse_position))
		{
			SetCapture(hWnd);
			ScrollBarControl(mouse_position);
			is_scroll_bar_click_ = true;
		}
	}
}

void FileManager::MouseMove(POINT mouse_position)
{
	if (is_scroll_bar_click_)
	{
		if (!PtInRect(&window_area_, mouse_position))
		{
			MouseUp();
		}

		if ((list_box_items_.size() * list_box_item_height_) > list_box_height_)
		{
			ScrollBarControl(mouse_position);
		}
	}
	

	if (PtInRect(&list_box_area_, mouse_position))
	{
		for (int i = 0; i < list_box_items_.size(); i++)
		{
			RECT area = { list_box_x_, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * scroll_bar_thumb_percent_) + (i * list_box_item_height_), list_box_x_ + list_box_width_, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * scroll_bar_thumb_percent_) + (i * list_box_item_height_) + list_box_item_height_ };
			if (PtInRect(&area, mouse_position))
			{
				list_item_select_ = i;
				InvalidateRect(hWnd, &file_manager_area_, FALSE);
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
	SolidBrush gray_brush(Color(255, 100, 100, 100));
	SolidBrush background_brush(Color(255, 33, 35, 39));

	Pen black_pen(Color(255, 0, 0, 0));

	FontFamily arial_font(L"Arial");
	Font font_style(&arial_font, 12, FontStyleBold, UnitPixel);

	// 파일 매니저
	x_ = window_area_.right - 400;
	y_ = 0;
	width_ = window_area_.right - x_;
	height_ = window_area_.bottom - 300;

	file_manager_area_ = { x_, y_, x_ + width_, y_ + height_ };

	graphics.FillRectangle(&background_brush, x_, y_, width_, height_);

	PointF header_font_position(x_ + 10, y_ + 9);
	graphics.DrawString(L"파일 관리자", -1, &font_style, header_font_position, &white_brush);

	// 리스트 박스
	list_box_width_ = 350;
	list_box_height_ = height_ - 50;
	list_box_x_ = x_ + 10;
	list_box_y_ = y_ + 40;

	list_box_area_ = { list_box_x_, list_box_y_, list_box_x_ + list_box_width_, list_box_y_ + list_box_height_ };

	graphics.FillRectangle(&white_brush, list_box_x_, list_box_y_, list_box_width_, list_box_height_);

	Region region(Rect(list_box_x_, list_box_y_, list_box_width_, list_box_height_));

	// 클리핑 마스크 시작
	graphics.SetClip(&region, CombineModeReplace);

	FileRefresh(current_path_);

	if (!list_box_items_.empty())
	{
		graphics.FillRectangle(&black_brush_, list_box_x_, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * scroll_bar_thumb_percent_) + (list_item_select_ * list_box_item_height_), list_box_width_, list_box_item_height_);
	}

	Image file_icon(L"Resources/FileIcon.png");
	Image image_file_icon(L"Resources/ImageFileIcon.png");
	Image folder_icon(L"Resources/FolderIcon.png");
	Image opened_folder_icon(L"Resources/OpenedFolderIcon.png");
	Image trash_icon(L"Resources/TrashIcon.png");

	WCHAR file_name_word[1024];
	WCHAR file_size_word[1024];
	StringFormat string_format;
	string_format.SetAlignment(StringAlignmentFar);

	for (int i = 0; i < list_box_items_.size(); i++)
	{
		PointF file_name_font_position(list_box_x_ + 35, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * scroll_bar_thumb_percent_) + 17 + (i * list_box_item_height_));
		PointF file_size_font_position(list_box_x_ + list_box_width_ - 35, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * scroll_bar_thumb_percent_) + 17 + (i * list_box_item_height_));
		wsprintf(file_name_word, L"%s", list_box_items_[i].file_name.c_str());
		wsprintf(file_size_word, L"%s", ConvertBytes(list_box_items_[i].file_size));

		
		if (fs::is_directory(list_box_items_[i].file_path))
		{
			if (fs::is_empty(list_box_items_[i].file_path))
			{
				graphics.DrawImage(&opened_folder_icon, list_box_x_ + 2.5, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * scroll_bar_thumb_percent_) + 10 + (i * list_box_item_height_), 30, 30);
			}
			else
			{
				graphics.DrawImage(&folder_icon, list_box_x_ + 2.5, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * scroll_bar_thumb_percent_) + 10 + (i * list_box_item_height_), 30, 30);
			}
		}
		else if (fs::is_regular_file(list_box_items_[i].file_path))
		{
			if (fs::path(list_box_items_[i].file_path).extension() == ".gb")
			{
				graphics.DrawImage(&image_file_icon, list_box_x_, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * scroll_bar_thumb_percent_) + 10 + (i * list_box_item_height_), 30, 30);
			}
			else
			{
				graphics.DrawImage(&file_icon, list_box_x_, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * scroll_bar_thumb_percent_) + 10 + (i * list_box_item_height_), 30, 30);
			}

			graphics.DrawString(file_size_word, -1, &font_style, file_size_font_position, &string_format, &black_brush);
		}

		graphics.DrawString(file_name_word, -1, &font_style, file_name_font_position, &black_brush);

		if (list_box_items_[i].file_path != "./Guests" && list_box_items_[i].file_path != fs::path(current_path_).parent_path())
		{
			graphics.DrawImage(&trash_icon, list_box_x_ + list_box_width_ - 35, list_box_y_ - (((list_box_items_.size() * list_box_item_height_) - list_box_height_) * scroll_bar_thumb_percent_) + 10 + (i * list_box_item_height_), 30, 30);
		}
	}

	// 클리핑 마스크 종료
	graphics.ResetClip();

	// 스크롤바
	scroll_bar_width_ = 30;
	scroll_bar_height_ = height_ - 50;
	scroll_bar_x_ = x_ + 360;
	scroll_bar_y_ = y_ + 40;;

	scroll_bar_thumb_ratio_ = (double)scroll_bar_height_ / (list_box_items_.size() * list_box_item_height_);

	scroll_bar_area_ = { scroll_bar_x_, scroll_bar_y_, scroll_bar_x_ + scroll_bar_width_, scroll_bar_y_ + scroll_bar_height_ };

	graphics.FillRectangle(&white_brush, scroll_bar_x_, scroll_bar_y_, scroll_bar_width_, scroll_bar_height_);
	// Scroll Bar Thumb
	if ((list_box_items_.size() * list_box_item_height_) > list_box_height_)
	{
		scroll_bar_thumb_height_ = scroll_bar_height_ * scroll_bar_thumb_ratio_;
		graphics.FillRectangle(&gray_brush, scroll_bar_x_ + 5, scroll_bar_y_ + 5 + (scroll_bar_thumb_percent_ / 1.0f) * (scroll_bar_height_ - scroll_bar_thumb_height_), scroll_bar_width_ - 10, round(scroll_bar_thumb_height_) - 10);
	}
}

void FileManager::FileRefresh(fs::path path)
{
	fs::path p(path);
	WCHAR root_path[256];
	wsprintf(root_path, L"%s\\Guests", fs::current_path().c_str());
	
	// 해당 경로가 존재하는지 확인
	if (fs::exists(p) && fs::is_directory(p))
	{
		fs::directory_iterator iter(p);

		list_box_items_.clear();

		// 현재 폴더가 Geusts가 아닐 경우 상위 폴더와 최상위 폴더 표시
		if (p != root_path)
		{
			list_box_items_.push_back({ ".", root_path, NULL });
			list_box_items_.push_back({ "..", p.parent_path(), NULL });
		}

		while (iter != fs::end(iter))
		{
			const fs::directory_entry& entry = *iter;
			list_box_items_.push_back({ entry.path().filename(), entry.path(), entry.file_size() });
			++iter;
		}
	}
	else
	{
		if (!fs::exists(root_path) && !fs::is_directory(root_path))
		{
			fs::create_directory(root_path);
		}

		FileRefresh(root_path);
	}
}
