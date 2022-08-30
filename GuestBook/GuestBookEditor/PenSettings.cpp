#include "PenSettings.h"

PenSettings::PenSettings()
{
    current_select_color = HSVToRGB(360.0f, 1.0f, 1.0f);
}

PenSettings::~PenSettings()
{
}

void PenSettings::Initialize(HWND hWnd)
{
    this->hWnd = hWnd;
}

bool PenSettings::IsOpen()
{
    return is_pen_settings_open_;
}

void PenSettings::MouseUp()
{
    is_palette_click_ = false;
    is_hue_slider_click_ = false;
    is_pen_size_slider_click_ = false;
}

void PenSettings::MouseDown(POINT mouse_position)
{
    if (PtInRect(&palette_area, mouse_position))
    {
        PaletteControl(mouse_position);
        is_palette_click_ = true;
    }
    else if (PtInRect(&hue_slider_area, mouse_position))
    {
        HueSliderControl(mouse_position);
        is_hue_slider_click_ = true;
    }
    else if (PtInRect(&pen_size_slider_area, mouse_position))
    {
        PenSizeSliderControl(mouse_position);
        is_pen_size_slider_click_ = true;
    }
}

void PenSettings::MouseMove(POINT mouse_position)
{
    if (is_palette_click_)
    {
        PaletteControl(mouse_position);
    }
    else if (is_hue_slider_click_)
    {
        HueSliderControl(mouse_position);
    }
    else if (is_pen_size_slider_click_)
    {
        PenSizeSliderControl(mouse_position);
    }
}

void PenSettings::PaletteControl(POINT mouse_position)
{
    s_ = min(max(((mouse_position.x - palette_x_) * 1.0f) / palette_width_, 0), 1.0f);
    v_ = min(max(((mouse_position.y - palette_y_) * 1.0f) / palette_height_, 0), 1.0f);
    InvalidateRect(hWnd, &pen_settings_area, FALSE);
}

void PenSettings::HueSliderControl(POINT mouse_position)
{
    h_ = min(max(((mouse_position.y - hue_slider_y_) * 360.0f) / hue_slider_height_, 0), 360.0f);
    InvalidateRect(hWnd, &pen_settings_area, FALSE);
}

void PenSettings::PenSizeSliderControl(POINT mouse_position)
{
    pen_size_ = min(max(((mouse_position.x - pen_size_slider_x_) * 10.0f) / pen_size_slider_width_, 0), 10.0f);
    InvalidateRect(hWnd, &pen_settings_area, FALSE);
}

void PenSettings::Open(POINT windows_size, POINT mouse_position)
{
    if (is_pen_settings_open_ == false)
    {
        x_ = mouse_position.x;
        y_ = mouse_position.y;

        // 윈도우 크기에 따른 위치 보정
        if (x_ > windows_size.x - width_)
        {
            x_ -= width_;
        }

        if (y_ > windows_size.y - height_)
        {
            y_ -= height_;
        }

        pen_settings_area = { x_, y_, x_ + width_, y_ + height_ };
    }
    
    is_pen_settings_open_ = !is_pen_settings_open_;

    InvalidateRect(hWnd, &pen_settings_area, FALSE);
}

void PenSettings::Draw(HDC hdc)
{
    Graphics graphics(hdc);

    // 배경 제거
    SetBkMode(hdc, TRANSPARENT);

    Pen black_outline(Color(255, 0, 0, 0));
    Pen white_outline(Color(200, 255, 255, 255));
    
    SolidBrush white_brush(Color(255, 255, 255, 255));
    SolidBrush black_brush(Color(255, 0, 0, 0));
    
    SolidBrush background(Color(255, 37, 41, 53));
    graphics.FillRectangle(&background, x_, y_, width_, height_);

    FontFamily arial_font(L"Arial");
    Font font_style(&arial_font, 12, FontStyleRegular, UnitPixel);

    PointF header_font_position(x_ + 20, y_ + 13);
    graphics.DrawString(L"Quick Pen Settings", -1, &font_style, header_font_position, &white_brush);

    // 팔레트
    palette_x_ = x_ + 20;
    palette_y_ = y_ + 40;

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

    graphics.DrawEllipse(&white_outline, palette_x_ + (s_ / 1.0f) * palette_width_ - 5, palette_y_ + (v_ / 1.0f) * palette_height_ - 5, 10, 10);
    
    // 색상 슬라이더
    hue_slider_x_ = palette_x_ + palette_width_ + 10;
    hue_slider_y_ = palette_y_;

    graphics.FillRectangle(&white_brush, hue_slider_x_, hue_slider_y_, hue_slider_width_, hue_slider_height_);
    
    Image hue_slider_image(L"Resources/Hue.png");
    graphics.DrawImage(&hue_slider_image, hue_slider_x_, hue_slider_y_, hue_slider_width_, hue_slider_height_);

    graphics.DrawEllipse(&white_outline, hue_slider_x_ + 10, hue_slider_y_ + (h_ / 360.0f) * hue_slider_height_ - 5, 10, 10);
    
    // 팬 크기 슬라이더
    pen_size_slider_x_ = palette_x_;
    pen_size_slider_y_ = palette_y_ + palette_height_ + 10;

    graphics.FillRectangle(&white_brush, pen_size_slider_x_, pen_size_slider_y_, pen_size_slider_width_, pen_size_slider_height_);
    
    LinearGradientBrush pen_size_slider_horizontal(
        Point(pen_size_slider_x_, pen_size_slider_y_),
        Point(pen_size_slider_x_ + pen_size_slider_width_, pen_size_slider_y_),
        Color(0, 255, 255, 255),
        Color(255, 200, 200, 200));
    
    graphics.FillRectangle(&pen_size_slider_horizontal, pen_size_slider_x_, pen_size_slider_y_, pen_size_slider_width_, pen_size_slider_height_);

    graphics.DrawEllipse(&white_outline, pen_size_slider_x_ + (pen_size_ / 10.0f) * pen_size_slider_width_ - 5, pen_size_slider_y_ + 10, 10, 10);
    
    WCHAR pen_size_word[1024];
    _stprintf_s(pen_size_word, L"%.lf", pen_size_);
    
    PointF pen_size_font_position(pen_size_slider_x_ + 5, pen_size_slider_y_ + 7);
    graphics.DrawString(pen_size_word, -1, &font_style, pen_size_font_position, &black_brush);

    current_select_color = HSVToRGB(360.0f - h_, s_, 1.0f - v_);

    // 색상 미리보기
    color_preview_x_ = pen_size_slider_x_ + pen_size_slider_width_ + 10;
    color_preview_y_ = pen_size_slider_y_;
    
    SolidBrush color_preview(current_select_color);
    graphics.FillRectangle(&color_preview, color_preview_x_, color_preview_y_, color_preview_width_, color_preview_height_);

    palette_area = { palette_x_ - 10, palette_y_ - 10, palette_x_ + palette_width_ + 10, palette_y_ + palette_height_ + 10 };
    hue_slider_area = { hue_slider_x_, hue_slider_y_ - 10, hue_slider_x_ + hue_slider_width_, hue_slider_y_ + hue_slider_height_ + 10 };
    pen_size_slider_area = { pen_size_slider_x_ - 10, pen_size_slider_y_, pen_size_slider_x_ + pen_size_slider_width_ + 10, pen_size_slider_y_ + pen_size_slider_height_ };

    WCHAR rgb_word[1024];
    wsprintf(rgb_word, L"RGB: %d, %d, %d", GetR(), GetG(), GetB());
    
    PointF rgb_font_position(pen_size_slider_x_, pen_size_slider_y_ + pen_size_slider_height_ + 13);
    graphics.DrawString(rgb_word, -1, &font_style, rgb_font_position, &white_brush);
}

double PenSettings::GetPenSize()
{
    return pen_size_;
}

// HSV 값을 RGB 값으로 변환
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

BYTE PenSettings::GetR()
{
    return current_select_color.GetR();
}

BYTE PenSettings::GetG()
{
    return current_select_color.GetG();
}

BYTE PenSettings::GetB()
{
    return current_select_color.GetB();
}
