#include "Button.h"

int Button(HWND hWnd, LPARAM lParam, HDC memdc, int status, RECT eraser, RECT clear, POINT st_pos)
{
    if (PtInRect(&eraser, st_pos))
    {
        //status = 0;
        MessageBox(hWnd, L"���찳 Ȱ��ȭ", L"�˸�", MB_OK);

        //���콺 ���� Ȱ��ȭ

        //����� ��ǥ �޾ƿ���
        st_pos.x = GET_X_LPARAM(lParam);
        st_pos.y = GET_Y_LPARAM(lParam);

        return 2;
    }
    else if (PtInRect(&clear, st_pos))
    {
        MessageBox(hWnd, L"ȭ�� ��ü �����", L"�˸�", MB_OK);
        
        RECT rect;
        GetClientRect(hWnd, &rect);
        FillRect(memdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
        return 0;
    }
    else
        return 1;
}