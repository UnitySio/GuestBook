#pragma once

#include "framework.h"

#include <vector>
#include <filesystem>
#include <fstream>

namespace fs = filesystem;

class FileManager
{
private:
    struct ListBoxItem
    {
        fs::path file_name;
        fs::path file_path;
        uintmax_t file_size;
    };

	HWND hWnd;
	RECT client_area_;
	RECT window_area_;

    RECT list_box_area_;

    // 생성될 x, y 좌표
    int x_;
    int y_;
    int width_ = 300;
    int height_ = 300;

    int list_box_x_;
    int list_box_y_;
    int list_box_width_;
    int list_box_height_;

    vector<ListBoxItem> list_box_items_;
    int item_select_ = 0;
    bool is_list_box_click_;
    bool is_item_click_;
    fs::path current_path_ = "./Guests";

    int list_box_item_height_ = 50;

    int scroll_bar_x_;
    int scroll_bar_y_;
    int scroll_bar_width_;
    int scroll_bar_height_;

    int scroll_bar_thumb_width_;
    int scroll_bar_thumb_height_;

    bool is_scroll_bar_click_;

    RECT scroll_bar_area_;
    
    double ratio_;
    double percent_;
    
    void UpdateWindowArea();
    wchar_t* ConvertBytes(uintmax_t bytes); // 단위 변환
public:
    FileManager(HWND hWnd);
    ~FileManager() = default;

    void MouseUp();
    void MouseDown(POINT mouse_position);
    void MouseMove(POINT mouse_position);
    void Draw(HDC hdc);

    void FileRefresh(fs::path path);

    vector<ListBoxItem> GetListBoxItem();
    
    int GetIndex();

    bool IsItemClick();
};

