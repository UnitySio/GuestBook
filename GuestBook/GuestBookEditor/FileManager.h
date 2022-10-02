#pragma once

#include "framework.h"

#include <vector>
#include <filesystem>
#include <fstream>

namespace fs = filesystem;

class FileManager
{
private:
    struct Item
    {
        fs::path file_name;
        fs::path file_path;
        uintmax_t file_size;
    };

    HWND hWnd;
    RECT client_area_;
    RECT window_area_;

    // 생성될 x, y 좌표
    int x_;
    int y_;
    int width_;
    int height_;

    int list_box_x_;
    int list_box_y_;
    int list_box_width_;
    int list_box_height_;

    RECT list_box_area_;

    vector<Item> items_;

    int list_item_select_;
    int list_item_height_;

    int scroll_bar_x_;
    int scroll_bar_y_;
    int scroll_bar_width_;
    int scroll_bar_height_;

    WCHAR root_path_[256];
    WCHAR current_path_[256];

    bool is_scroll_bar_click_;

    RECT scroll_bar_area_;
    RECT file_manager_area_;

    double scroll_bar_thumb_ratio_;
    double scroll_bar_thumb_percent_;
    double scroll_bar_thumb_height_;

    void UpdateWindowArea();
    void ScrollBarControl(POINT mouse_position);

    LPCWSTR ConvertBytes(uintmax_t bytes); // 단위 변환
public:
    FileManager(HWND hWnd);
    ~FileManager() = default;

    void MouseUp();
    void MouseDown(POINT mouse_position);
    void MouseDoubleDown(POINT mouse_position);
    void MouseMove(POINT mouse_position);
    void MouseWheel(float direction);
    void Draw(HDC hdc);

    void FileRefresh(fs::path path);

    LPCWSTR GetRootPath();
    LPCWSTR GetCurrentPath();

    int GetWidth();
    int GetHeight();
};

