#include "Timeline.h"

Timeline::Timeline(HWND hWnd)
{
	this->hWnd = hWnd;
	UpdateWindowArea();
}

Timeline::~Timeline()
{

}

void Timeline::UpdateWindowArea()
{
	GetClientRect(hWnd, &client_area);
	window_area = { 0, 0, client_area.right - client_area.left, client_area.bottom - client_area.top };
}

void Timeline::Draw(HDC hdc)
{
	UpdateWindowArea();

	Graphics graphics(hdc);

	// 배경 제거
	SetBkColor(hdc, TRANSPARENT);

	SolidBrush brush(Color(255, 0, 0, 0));

	graphics.FillRectangle(&brush, 0, 100, window_area.right, 10);
}