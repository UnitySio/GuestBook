﻿#pragma once

#include "framework.h"

class QuickPanel
{
private:
    HWND hWnd;
    RECT client_area;
    RECT window_area;

    // 생성될 x, y 좌표
    int x_;
    int y_;
    int width_ = 420; // 280
    int height_ = 300;
    
    double h_; // 색상(Hue)
    double s_ = 1; // 채도(Saturation)
    double v_; // 명도(Value)
    double pen_size_; // 팬 크기

    // 영역
    RECT quick_panel_area;
    RECT palette_area;
    RECT hue_slider_area;
    RECT pen_size_slider_area;

    // 팔레트
    int palette_x_;
    int palette_y_;
    int palette_width_ = 200;
    int palette_height_ = 200;

    // 색상 슬라이더
    int hue_slider_x_;
    int hue_slider_y_;
    int hue_slider_width_ = 30;
    int hue_slider_height_ = 200;

    // 팬 크기 슬라이더
    int pen_size_slider_x_;
    int pen_size_slider_y_;
    int pen_size_slider_width_ = 200;
    int pen_size_slider_height_ = 30;

    // 색상 미리보기
    int color_preview_x_;
    int color_preview_y_;
    int color_preview_width_ = 30;
    int color_preview_height_ = 30;
    
    Color current_select_color = HSVToRGB(360.0f - h_, s_, 1.0f - v_);;
    
    bool is_quick_panel_open_;
    bool is_palette_click_;
    bool is_hue_slider_click_;
    bool is_pen_size_slider_click_;

    void UpdateWindowArea();
    void PaletteControl(POINT mouse_position);
    void HueSliderControl(POINT mouse_position);
    void PenSizeSliderControl(POINT mouse_position);
public:
    QuickPanel(HWND hWnd);
    ~QuickPanel();
    
    bool IsOpen();

    void MouseUp();
    void MouseDown(POINT mouse_position);
    void MouseMove(POINT mouse_position);
    void Open(POINT mouse_position);
    void Draw(HDC hdc);

    int GetPenSize();

    Color HSVToRGB(double h, double s, double v);

    BYTE GetR();
    BYTE GetG();
    BYTE GetB();
};
