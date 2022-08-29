#pragma once
#include "framework.h"

class PenSettings
{
private:
    // 생성될 x, y 좌표
    int x_;
    int y_;
    int width_ = 300;
    int height_ = 300;
    
    double h_; // 색조(Hue)
    double s_ = 1; // 채도(Saturation)
    double b_; // 밝기(Brightness)
    double pen_size_; // 팬 크기

    // 팔레트
    int palette_x_;
    int palette_y_;
    int palette_width_ = 200;
    int palette_height_ = 200;

    // 색조 슬라이더
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
    
    bool is_pen_settings_open_;
    bool is_palette_click_;
    bool is_hue_slider_click_;
    bool is_pen_size_slider_click_;
public:
    PenSettings();
    ~PenSettings();
    
    bool IsOpen();
    
    void Up();
    void Down(POINT mouse_position);
    void Move(POINT mouse_position);
    void Open(POINT mouse_position);
    void Draw(POINT windows_size, Graphics &graphics);

    Color HSVToRGB(double h, double s, double v);
};
