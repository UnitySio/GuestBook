#include "QuickPanel.h"

QuickPanel::QuickPanel(HWND hWnd)
{
    this->hWnd = hWnd;
    UpdateWindowsArea();
}

QuickPanel::~QuickPanel()
{
}

bool QuickPanel::IsOpen()
{
    return is_quick_panel_open_;
}

void QuickPanel::UpdateWindowsArea()
{
    GetClientRect(hWnd, &client_area);
    windows_area = { 0, 0, client_area.right - client_area.left, client_area.bottom - client_area.top };
}


void QuickPanel::MouseUp()
{
    if (is_quick_panel_open_)
    {
        ReleaseCapture();
        is_palette_click_ = false;
        is_hue_slider_click_ = false;
        is_pen_size_slider_click_ = false;
    }
}

void QuickPanel::MouseDown(POINT mouse_position)
{
    if (is_quick_panel_open_)
    {
        SetCapture(hWnd);
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
}

void QuickPanel::MouseMove(POINT mouse_position)
{
    if (is_quick_panel_open_)
    {
        if (PtInRect(&windows_area, mouse_position) == false)
        {
            MouseUp();
        }
        
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
}

void QuickPanel::PaletteControl(POINT mouse_position)
{
    s_ = min(max(((mouse_position.x - palette_x_) * 1.0f) / palette_width_, 0), 1.0f);
    v_ = min(max(((mouse_position.y - palette_y_) * 1.0f) / palette_height_, 0), 1.0f);
    InvalidateRect(hWnd, &quick_panel_area, FALSE);
}

void QuickPanel::HueSliderControl(POINT mouse_position)
{
    h_ = min(max(((mouse_position.y - hue_slider_y_) * 360.0f) / hue_slider_height_, 0), 360.0f);
    InvalidateRect(hWnd, &quick_panel_area, FALSE);
}

void QuickPanel::PenSizeSliderControl(POINT mouse_position)
{
    pen_size_ = min(max(((mouse_position.x - pen_size_slider_x_) * 30.0f) / pen_size_slider_width_, 0), 30.0f);
    InvalidateRect(hWnd, &quick_panel_area, FALSE);
}

void QuickPanel::Open(POINT mouse_position)
{
    if (is_quick_panel_open_ == false)
    {
        x_ = mouse_position.x;
        y_ = mouse_position.y;

        // 윈도우 크기에 따른 위치 보정
        if (x_ > windows_area.right - width_)
        {
            x_ -= width_;
        }

        if (y_ > windows_area.bottom - height_)
        {
            y_ -= height_;
        }

        quick_panel_area = { x_, y_, x_ + width_, y_ + height_ };
    }
    
    is_quick_panel_open_ = !is_quick_panel_open_;

    InvalidateRect(hWnd, &quick_panel_area, FALSE);
}

void QuickPanel::Draw(HDC hdc)
{
    if (is_quick_panel_open_)
    {
        UpdateWindowsArea();

        Graphics graphics(hdc);

        // 배경 제거
        SetBkMode(hdc, TRANSPARENT);

        Pen black_pen(Color(255, 0, 0, 0));
        Pen white_pen(Color(255, 255, 255, 255));

        SolidBrush white_brush(Color(255, 255, 255, 255));
        SolidBrush black_brush(Color(255, 0, 0, 0));
        SolidBrush white_alpha_brush(Color(50, 255, 255, 255));
        SolidBrush background_brush(Color(255, 33, 35, 39));
        
        graphics.FillRectangle(&background_brush, x_, y_, width_, height_);

        FontFamily arial_font(L"Arial");
        Font font_style(&arial_font, 12, FontStyleRegular, UnitPixel);

        PointF header_font_position(x_ + 20, y_ + 13);
        graphics.DrawString(L"Quick Panel", -1, &font_style, header_font_position, &white_brush);

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

        graphics.FillEllipse(&white_alpha_brush, palette_x_ + (s_ / 1.0f) * palette_width_ - 10, palette_y_ + (v_ / 1.0f) * palette_height_ - 10, 20, 20);
        graphics.DrawEllipse(&white_pen, palette_x_ + (s_ / 1.0f) * palette_width_ - 10, palette_y_ + (v_ / 1.0f) * palette_height_ - 10, 20, 20);

        // 색상 슬라이더
        hue_slider_x_ = palette_x_ + palette_width_ + 10;
        hue_slider_y_ = palette_y_;

        graphics.FillRectangle(&white_brush, hue_slider_x_, hue_slider_y_, hue_slider_width_, hue_slider_height_);

        Image hue_slider_image(L"Resources/Hue.png");
        graphics.DrawImage(&hue_slider_image, hue_slider_x_, hue_slider_y_, hue_slider_width_, hue_slider_height_);

        Point left_points[] = {
            Point(hue_slider_x_, hue_slider_y_ + (h_ / 360.0f) * hue_slider_height_),
            Point(hue_slider_x_ - 5, hue_slider_y_ + (h_ / 360.0f) * hue_slider_height_ + 5),
            Point(hue_slider_x_ - 5, hue_slider_y_ + (h_ / 360.0f) * hue_slider_height_ - 5) };

        graphics.FillPolygon(&white_brush, left_points, 3);

        Point right_points[] = {
            Point(hue_slider_x_ + hue_slider_width_, hue_slider_y_ + (h_ / 360.0f) * hue_slider_height_),
            Point(hue_slider_x_ + hue_slider_width_ + 5, hue_slider_y_ + (h_ / 360.0f) * hue_slider_height_ + 5),
            Point(hue_slider_x_ + hue_slider_width_ + 5, hue_slider_y_ + (h_ / 360.0f) * hue_slider_height_ - 5) };

        graphics.FillPolygon(&white_brush, right_points, 3);

        // 팬 크기 슬라이더
        pen_size_slider_x_ = palette_x_;
        pen_size_slider_y_ = palette_y_ + palette_height_ + 10;

        LinearGradientBrush pen_size_slider_horizontal(
            Point(pen_size_slider_x_, pen_size_slider_y_),
            Point(pen_size_slider_x_ + pen_size_slider_width_, pen_size_slider_y_),
            Color(255, 255, 255, 255),
            Color(255, 200, 200, 200));

        graphics.FillRectangle(&pen_size_slider_horizontal, pen_size_slider_x_, pen_size_slider_y_, pen_size_slider_width_, pen_size_slider_height_);

        Point top_points[] = {
            Point(pen_size_slider_x_ + (pen_size_ / 30.0f) * pen_size_slider_width_, pen_size_slider_y_),
            Point(pen_size_slider_x_ + (pen_size_ / 30.0f) * pen_size_slider_width_ - 5, pen_size_slider_y_ - 5),
            Point(pen_size_slider_x_ + (pen_size_ / 30.0f) * pen_size_slider_width_ + 5, pen_size_slider_y_ - 5) };

        graphics.FillPolygon(&white_brush, top_points, 3);

        graphics.DrawLine(&white_pen, pen_size_slider_x_, pen_size_slider_y_ + 5, pen_size_slider_x_ + 10, pen_size_slider_y_ + 5);

        Point bottom_points[] = {
            Point(pen_size_slider_x_ + (pen_size_ / 30.0f) * pen_size_slider_width_, pen_size_slider_y_ + pen_size_slider_height_),
            Point(pen_size_slider_x_ + (pen_size_ / 30.0f) * pen_size_slider_width_ - 5, pen_size_slider_y_ + pen_size_slider_height_ + 5),
            Point(pen_size_slider_x_ + (pen_size_ / 30.0f) * pen_size_slider_width_ + 5, pen_size_slider_y_ + pen_size_slider_height_ + 5) };

        graphics.FillPolygon(&white_brush, bottom_points, 3);

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

        WCHAR hsv_word[1024];
        wsprintf(hsv_word, L"HSV: %d°, %d%%, %d%%", (int)trunc(360.0f - h_), (int)trunc(s_ * 100), (int)trunc((1.0f - v_) * 100));

        PointF hsv_font_position(hue_slider_x_ + hue_slider_width_ + 10, hue_slider_y_);
        graphics.DrawString(hsv_word, -1, &font_style, hsv_font_position, &white_brush);
        
        WCHAR rgb_word[1024];
        wsprintf(rgb_word, L"RGB: %d, %d, %d", GetR(), GetG(), GetB());

        PointF rgb_font_position(hue_slider_x_ + hue_slider_width_ + 10, hue_slider_y_ + 13);
        graphics.DrawString(rgb_word, -1, &font_style, rgb_font_position, &white_brush);
        
        WCHAR hex_word[1024];
        wsprintf(hex_word, L"HEX: #%02x%02x%02x", GetR(), GetG(), GetB());

        PointF hex_font_position(hue_slider_x_ + hue_slider_width_ + 10, hue_slider_y_ + 26);
        graphics.DrawString(hex_word, -1, &font_style, hex_font_position, &white_brush);
        
        PointF preview_font_position(hue_slider_x_ + hue_slider_width_ + 10, hue_slider_y_ + 90);
        graphics.DrawString(L"Preview", -1, &font_style, preview_font_position, &white_brush);
        
        graphics.FillRectangle(&white_brush, hue_slider_x_ + hue_slider_width_ + 10, hue_slider_y_ + 110, 130, 130);
        
        Pen pen(current_select_color, pen_size_);
        graphics.DrawLine(&pen, hue_slider_x_ + hue_slider_width_ + 30, hue_slider_y_ + 180, hue_slider_x_ + hue_slider_width_ + 120, hue_slider_y_ + 180 );
    }
}

double QuickPanel::GetPenSize()
{
    return pen_size_; // 값 보정 작업 예정
}

// HSV 값을 RGB 값으로 변환
Color QuickPanel::HSVToRGB(double h, double s, double v)
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

BYTE QuickPanel::GetR()
{
    return current_select_color.GetR();
}

BYTE QuickPanel::GetG()
{
    return current_select_color.GetG();
}

BYTE QuickPanel::GetB()
{
    return current_select_color.GetB();
}
