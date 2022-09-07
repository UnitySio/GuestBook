#pragma once
#include "framework.h"

class Timeline
{
private:
    HWND hWnd;
    RECT client_area_;
    RECT window_area_;

    // ������ x, y ��ǥ
    int x_;
    int y_;
    int width_;
    int height_;

    double time_ = 0;
    double max_time_ = 0;

    // ����
    RECT timeline_area_;

    // ���α׷���
    int progress_x_;
    int progress_y_;
    int progress_width_;
    int progress_height_;

    bool is_progress_click_;
    
    void UpdateWindowArea();
    void ProgressControl(POINT mouse_position);
public:
    Timeline(HWND hWnd);
    ~Timeline();

    void MouseUp();
    void MouseDown(POINT mouse_position);
    void MouseMove(POINT mouse_position);
    void UpdateMaxTime(double time);
    void Play();
    void Draw(HDC hdc);
};

