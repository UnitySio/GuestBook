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
        fs::path file_path;
    };

	HWND hWnd;
	RECT client_area_;
	RECT window_area_;

    // »ý¼ºµÉ x, y ÁÂÇ¥
    int x_;
    int y_;
    int width_ = 300;
    int height_ = 300;

    vector<ListBoxItem> listBoxItems_;
    int item_select_ = 0;

    void UpdateWindowArea();
public:
    FileManager(HWND hWnd);
    ~FileManager() = default;

    void MouseUp();
    void MouseDown(POINT mouse_position);
    void MouseMove(POINT mouse_position);
    void Draw(HDC hdc);

    void FileRefresh();

    vector<ListBoxItem> GetListBoxItem();
    int GetIndex();
};

