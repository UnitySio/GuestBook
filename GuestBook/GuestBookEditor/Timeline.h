#pragma once
#include "framework.h"

class Timeline
{
private:
    HWND hWnd;
    RECT client_area_;
    RECT window_area_;

    int x_;
    int y_;
    int width_;
    int height_;
    
    void UpdateWindowArea();
public:
    Timeline(HWND hWnd);
    ~Timeline();

    void Draw(HDC hdc);
};

