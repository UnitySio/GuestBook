#pragma once
#include "framework.h"

class Timeline
{
private:
    HWND hWnd;
    RECT client_area;
    RECT window_area;
    
    void UpdateWindowArea();
public:
    Timeline(HWND hWnd);
    ~Timeline();

    void Draw(HDC hdc);
};

