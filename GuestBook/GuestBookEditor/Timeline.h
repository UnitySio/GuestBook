#pragma once
#include "framework.h"

class Timeline
{
private:
    HWND hWnd;

    // ������ x, y ��ǥ
    int x_;
    int y_;
    int width_;
    int height_;

    // ����
    RECT timeline_area_;

    double time_;
    double max_time_;

    bool is_list_box_click_;
    bool is_playing_;

    // Ű ������
    int list_box_x_;
    int list_box_y_;
    int list_box_width_;
    int list_box_height_;

    int scroll_bar_x_;
    int scroll_bar_y_;
    int scroll_bar_width_;
    int scroll_bar_height_;

    bool is_scroll_bar_click_;

    RECT list_box_area_;
    RECT scroll_bar_area_;

    double scroll_bar_thumb_ratio_;
    double scroll_bar_thumb_percent_;
    double scroll_bar_thumb_height_;

    void KeyFrameControl(POINT mouse_position);
    void ScrollBarControl(POINT mouse_position);
public:
    Timeline(HWND hWnd);
    ~Timeline() = default;

    void MouseUp();
    void MouseDown(POINT mouse_position);
    void MouseMove(POINT mouse_position);
    void MouseWheel(POINT mouse_position, float direction);
    void AddTime(double time);
    void UpdateMaxTime(double time);
    void Draw(HDC hdc);
    void Play();
    RECT GetTimelineArea();

    int GetWidth();
    int GetHeight();
    int GetTime();
    int GetMaxTime();

    bool OnPlay();
};
