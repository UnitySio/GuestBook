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
	GetClientRect(hWnd, &client_area_);
	window_area_ = { 0, 0, client_area_.right - client_area_.left, client_area_.bottom - client_area_.top };
}

void Timeline::Draw(HDC hdc)
{
	UpdateWindowArea();

	Graphics graphics(hdc);

	// 배경 제거
	SetBkColor(hdc, TRANSPARENT);

	SolidBrush brush(Color(255, 0, 0, 0));
}