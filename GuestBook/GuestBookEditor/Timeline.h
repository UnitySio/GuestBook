#pragma once
#include "framework.h"

class Timeline
{
private:
    HWND hWnd;
    RECT client_area_;
    RECT window_area_;

    // 생성될 x, y 좌표
    int x_;
    int y_;
    int width_;
    int height_;

    // 영역
    RECT timeline_area_;

    // 프로그래스
    int progress_x_;
    int progress_y_;
    int progress_width_;
    int progress_height_;

    double time_ = 0;
    double max_time_ = 0;

    bool is_progress_click_;
    bool is_playing_;
    
    void UpdateWindowArea();
    void ProgressControl(POINT mouse_position);
public:
    Timeline(HWND hWnd);
    ~Timeline();

    void MouseUp();
    void MouseDown(POINT mouse_position);
    void MouseMove(POINT mouse_position);
    void AddTime(double time);
    void UpdateMaxTime(double time);
    void Draw(HDC hdc);
    void Play();

    int GetTime();
    int GetMaxTime();

    bool IsPlaying();
};

