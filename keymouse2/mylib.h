#pragma once
#include <stdio.h>
#include <Gdiplus.h>
#include <atlimage.h>
#include <atlstr.h>
#include <time.h>
#include <sys/timeb.h>
#include "mylog.h"


//屏幕截图功能实现
void ScreenCapture(LPSTR filename, LPRECT lpRect)
{

	HDC hdcSrc = GetDC(NULL);
	int nBitPerPixel = GetDeviceCaps(hdcSrc, BITSPIXEL);


	int nWidth = GetDeviceCaps(hdcSrc, HORZRES);
	int nHeight = GetDeviceCaps(hdcSrc, VERTRES);
	CImage image;
	image.Create(nWidth, nHeight, nBitPerPixel);
	int startx = lpRect->left;
	int starty = lpRect->top;

	if (lpRect->right - lpRect->left > 0)
		nWidth = lpRect->right - lpRect->left;
	if (lpRect->bottom - lpRect->top > 0)
		nHeight = lpRect->bottom - lpRect->top;


	BitBlt(image.GetDC(), 0, 0, nWidth, nHeight, hdcSrc, startx, starty, SRCCOPY);
	ReleaseDC(NULL, hdcSrc);
	image.ReleaseDC();
	image.Save(L"c:/ScreenShot.png", Gdiplus::ImageFormatPNG);//ImageFormatJPEG

}

void WindowCapture(CString windowName) {
	HWND hwnd = FindWindow(NULL, windowName);
	SetActiveWindow(hwnd); //父窗口置为活动窗口
	SetForegroundWindow(hwnd);
	SwitchToThisWindow(hwnd, true);
	Sleep(2000);
	RECT rect;
	::GetWindowRect(hwnd, &rect);
	ScreenCapture("c:/screen.jpg", &rect);
}


void moveTo(int x0, int y0, int x1, int y1) {
   for (int x=x0;x<x1;x++){
	   int y = y0 + (y1 - y0)* (x-x0) / (x1-x0);
	   SetCursorPos(x, y);
   }
}
void clickAt(int x, int y) {
	CPoint pointOld;
	GetCursorPos(&pointOld);

	char s[200];
	sprintf_s(s, "move mouse and click at x: %d, y:%d \n", x, y);
	printf(s);
	mylog(s, "_");
//	moveTo(pointOld.x, pointOld.y ,  x, y);
	SetCursorPos(x, y);

	Sleep(500);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);//按下鼠标左键
	Sleep(10);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);//松开鼠标左键
	Sleep(20);
}
void string2clipboard(char*s) {
	CString source;
	//put your text in source
	if (OpenClipboard(NULL))
	{
		HGLOBAL clipbuffer;
		int len = 100;
		char *buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, len + 1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy_s(buffer, len, s);
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT, clipbuffer);
		CloseClipboard();
	}

}
void inputText3(char*s,char* inputtext) {
	if (inputtext !=NULL)
		string2clipboard(inputtext);
	else 
		string2clipboard(s);
 /*	keybd_event('a', 0, 0, 0);
	keybd_event('a', 0, KEYEVENTF_KEYUP, 0);
	keybd_event(65, MapVirtualKey(0x41, 0), 0, 0);
	Sleep(300);
	keybd_event(65, MapVirtualKey(0x41, 0), KEYEVENTF_KEYUP, 0);
	*/
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event('V', 0, 0, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
	Sleep(3000);
	keybd_event(VK_RETURN, 0, 0, 0);
	keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);

}

void inputText(char* s) {
	HWND hWnd = (HWND)0x00730466;
	//	SetCursorPos(613, 343);
	// SetCursorPos(m_ddcoord.loginname_x, m_ddcoord.loginname_y);
	//	CString sText = "text string";

	USES_CONVERSION;
	WCHAR buf[1024];// = _T("咚咚测试1");
	int length = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
	ZeroMemory(buf, (length + 1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, s, -1, buf, length);
	//	clickAt(613, 343);
	hWnd = GetForegroundWindow(); // 得到当前窗口 
	hWnd = (HWND)0x00730466;
	int len = lstrlen(buf);
	for (int i = 0; i< len; i++)
	{
		//WM_CHAR 不打开输入法
		::PostMessage(hWnd, WM_CHAR, buf[i], 0);
	}
}
void mouseWheel(int x, int y , int wheeltime=6) {
	HWND hWnd = GetForegroundWindow(); // 得到当前窗口 
	for (int i = 0; i < wheeltime; i++){
		mouse_event(MOUSEEVENTF_WHEEL, 0, 0, -120, 0);
		Sleep(1000);
	}
}

void doubleclickAt(int x, int y) {
	char s[200];
	sprintf_s(s, "move mouse and click at x: %d, y:%d \n", x, y);

	mylog(s, "_");
	SetCursorPos(x, y);
	//	Sleep(500);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);//按下鼠标左键
	Sleep(10);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);//松开鼠标左键
	Sleep(20);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);//按下鼠标左键
	Sleep(10);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);//松开鼠标左键
	Sleep(20);
}



// 上拉
void pullup(int x1, int y1, int x2,int y2){
	char s[200];
	sprintf_s(s, "pull up from  x1: %d, y1:%d  to x2:%d y2:%d \n", x1 , y1,x2,y2);
    mylog(s, "_");
	
	SetCursorPos(x1, y1);
	//	Sleep(500);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);//按下鼠标左键
	Sleep(100);
	for (int y = y1; y > y2; y--){
		SetCursorPos(x2, y);Sleep(1);
	}
	//SetCursorPos(x2, y2);

	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);//松开鼠标左键
	Sleep(20);
}