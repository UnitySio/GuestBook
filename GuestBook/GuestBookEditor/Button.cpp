#include "Button.h"

int Button(HWND hWnd, LPARAM lParam, HDC memdc, int status, RECT eraser, RECT clear, POINT st_pos)
{
    if (PtInRect(&eraser, st_pos))
    {
        //status = 0;
        MessageBox(hWnd, L"지우개 활성화", L"알림", MB_OK);

        //마우스 상태 활성화

        //지우는 좌표 받아오기
        st_pos.x = GET_X_LPARAM(lParam);
        st_pos.y = GET_Y_LPARAM(lParam);

        return 2;
    }
    else if (PtInRect(&clear, st_pos))
    {
        MessageBox(hWnd, L"화면 전체 지우기", L"알림", MB_OK);
        
        RECT rect;
        GetClientRect(hWnd, &rect);
        FillRect(memdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
        return 0;
    }
    else
        return 1;
}