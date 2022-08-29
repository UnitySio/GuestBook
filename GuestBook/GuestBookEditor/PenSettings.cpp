#include "PenSettings.h"

#include <cmath>

PenSettings::PenSettings()
{
}

PenSettings::~PenSettings()
{
}

bool PenSettings::IsOpen()
{
    return is_pen_settings_open_ == true;
}

void PenSettings::Up()
{
    is_palette_click_ = false;
    is_hue_slider_click_ = false;
    is_pen_size_slider_click_ = false;
}


void PenSettings::Down(POINT mouse_position)
{
    RECT palette_area = { palette_x_, palette_y_, palette_x_ + palette_width_, palette_y_ + palette_height_ };
    RECT hue_slider_area = { hue_slider_x_, hue_slider_y_, hue_slider_x_ + hue_slider_width_, hue_slider_y_ + hue_slider_height_ };
    RECT pen_size_slider_area = { pen_size_slider_x_, pen_size_slider_y_, pen_size_slider_x_ + pen_size_slider_width_, pen_size_slider_y_ + pen_size_slider_height_ };

    if (PtInRect(&palette_area, mouse_position))
    {
        s_ = min(max(((mouse_position.x - palette_x_) * 1.0f) / palette_width_, 0), 1.0f);
        b_ = min(max(((mouse_position.y - palette_y_) * 1.0f) / palette_height_, 0), 1.0f);
        is_palette_click_ = true;
    }
    else if (PtInRect(&hue_slider_area, mouse_position))
    {
        h_ = min(max(((mouse_position.y - hue_slider_y_) * 360.0f) / hue_slider_height_, 0), 360.0f);
        is_hue_slider_click_ = true;
    }
    else if (PtInRect(&pen_size_slider_area, mouse_position))
    {
        pen_size_ = min(max(((mouse_position.x - pen_size_slider_x_) * 10.0f) / pen_size_slider_width_, 0), 10.0f);
        is_pen_size_slider_click_ = true;
    }
}

void PenSettings::Move(POINT mouse_position)
{
    RECT palette_area = { palette_x_, palette_y_, palette_x_ + palette_width_, palette_y_ + palette_height_ };
    RECT hue_slider_area = { hue_slider_x_, hue_slider_y_, hue_slider_x_ + hue_slider_width_, hue_slider_y_ + hue_slider_height_ };
    RECT pen_size_slider_area = { pen_size_slider_x_, pen_size_slider_y_, pen_size_slider_x_ + pen_size_slider_width_, pen_size_slider_y_ + pen_size_slider_height_ };

    if (is_palette_click_)
    {
        s_ = min(max(((mouse_position.x - palette_x_) * 1.0f) / palette_width_, 0), 1.0f);
        b_ = min(max(((mouse_position.y - palette_y_) * 1.0f) / palette_height_, 0), 1.0f);
    }
    else if (is_hue_slider_click_)
    {
        h_ = min(max(((mouse_position.y - hue_slider_y_) * 360.0f) / hue_slider_height_, 0), 360.0f);
    }
    else if (is_pen_size_slider_click_)
    {
        pen_size_ = min(max(((mouse_position.x - pen_size_slider_x_) * 10.0f) / pen_size_slider_width_, 0), 10.0f);
    }
}


void PenSettings::Open(POINT mouse_position)
{
    if (is_pen_settings_open_ == false)
    {
        x_ = mouse_position.x;
        y_ = mouse_position.y;
    }
    
    is_pen_settings_open_ = !is_pen_settings_open_;
}

void PenSettings::Draw(POINT windows_size, Graphics &graphics)
{
    // 윈도우 크기에 따른 위치 보정
    if (x_ > windows_size.x - width_)
    {
        x_ -= width_;
    }

    if (y_ > windows_size.y - height_)
    {
        y_ -= height_;
    }
    
    Pen black_outline(Color(255, 0, 0, 0));
    
    SolidBrush white_background(Color(255, 255, 255, 255));

    graphics.FillRectangle(&white_background, x_, y_, width_, height_);
    graphics.DrawRectangle(&black_outline, x_ - 1, y_ - 1, width_ + 1, height_ + 1);

    // 팔레트
    palette_x_ = x_ + 20;
    palette_y_ = y_ + 20;

    SolidBrush palette_background(Color(255, 255, 255, 255));
    graphics.FillRectangle(&palette_background, palette_x_, palette_y_, palette_width_, palette_height_);
    
    LinearGradientBrush palette_horizontal(
        Point(palette_x_, palette_y_),
        Point(palette_x_ + palette_width_, palette_y_),
        Color(0, 255, 255, 255),
        HSVToRGB(360.0f - h_, 1, 1));
    
    graphics.FillRectangle(&palette_horizontal, palette_x_, palette_y_, palette_width_, palette_height_);
    
    LinearGradientBrush palette_vertical(
        Point(palette_x_, palette_y_ + palette_height_),
        Point(palette_x_, palette_y_),
        Color(255, 0, 0, 0),
        Color(0, 255, 255, 255));
    
    graphics.FillRectangle(&palette_vertical, palette_x_, palette_y_, palette_width_, palette_height_);
    graphics.DrawRectangle(&black_outline, palette_x_ - 1, palette_y_ - 1, palette_width_ + 1, palette_height_ + 1);

    graphics.DrawEllipse(&black_outline, palette_x_ + (s_ / 1.0f) * palette_width_ - 5, palette_y_ + (b_ / 1.0f) * palette_height_ - 5, 10, 10);
    
    // 색조 슬라이더
    hue_slider_x_ = palette_x_ + palette_width_ + 30;
    hue_slider_y_ = palette_y_;

    graphics.FillRectangle(&white_background, hue_slider_x_, hue_slider_y_, hue_slider_width_, hue_slider_height_);
    
    Image hue_slider_image(L"Resources/Hue.png");
    graphics.DrawImage(&hue_slider_image, hue_slider_x_, hue_slider_y_, hue_slider_width_, hue_slider_height_);
    graphics.DrawRectangle(&black_outline, hue_slider_x_ - 1, hue_slider_y_ - 1, hue_slider_width_ + 1, hue_slider_height_ + 1);

    graphics.DrawEllipse(&black_outline, hue_slider_x_ + 10, hue_slider_y_ + (h_ / 360.0f) * hue_slider_height_ - 5, 10, 10);
    
    // 팬 크기 슬라이더
    pen_size_slider_x_ = palette_x_;
    pen_size_slider_y_ = palette_y_ + palette_height_ + 30;

    graphics.FillRectangle(&white_background, pen_size_slider_x_, pen_size_slider_y_, pen_size_slider_width_, pen_size_slider_height_);
    
    LinearGradientBrush pen_size_slider_horizontal(
        Point(pen_size_slider_x_, pen_size_slider_y_),
        Point(pen_size_slider_x_ + pen_size_slider_width_, pen_size_slider_y_),
        Color(0, 255, 255, 255),
        Color(255, 0, 0, 0));
    
    graphics.FillRectangle(&pen_size_slider_horizontal, pen_size_slider_x_, pen_size_slider_y_, pen_size_slider_width_, pen_size_slider_height_);
    graphics.DrawRectangle(&black_outline, pen_size_slider_x_ - 1, pen_size_slider_y_ - 1, pen_size_slider_width_ + 1, pen_size_slider_height_ + 1);

    graphics.DrawEllipse(&black_outline, pen_size_slider_x_ + (pen_size_ / 10.0f) * pen_size_slider_width_ - 5, pen_size_slider_y_ + 10, 10, 10);
    
    // 색상 미리보기
    color_preview_x_ = pen_size_slider_x_ + pen_size_slider_width_ + 30;
    color_preview_y_ = pen_size_slider_y_;
    
    SolidBrush color_preview(HSVToRGB(360.0f - h_, s_, 1.0f - b_));
    graphics.FillRectangle(&color_preview, color_preview_x_, color_preview_y_, color_preview_width_, color_preview_height_);
    graphics.DrawRectangle(&black_outline, color_preview_x_ - 1, color_preview_y_ - 1, color_preview_width_ + 1, color_preview_height_ + 1);
}

Color PenSettings::HSVToRGB(double h, double s, double v)
{
    
    double r = 0;
    double g = 0;
    double b = 0;

    if (s == 0)
    {
        r = v;
        g = v;
        b = v;
    }
    else
    {
        int i;
        double f, p, q, t;

        if (h == 360)
        {
            h = 0;
        }
        else
        {
            h = h / 60;
        }

        i = (int)trunc(h);
        f = h - i;

        p = v * (1.0 - s);
        q = v * (1.0 - (s * f));
        t = v * (1.0 - (s * (1.0 - f)));

        switch (i)
        {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        default:
            r = v;
            g = p;
            b = q;
            break;
        }
    }
    
    return Color(255, (BYTE)(r * 255), (BYTE)(g * 255), (BYTE)(b * 255));
}

