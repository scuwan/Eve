#pragma once
#import "lw.dll" no_namespace named_guids
#include "IWnd.h"
#include <atlbase.h>
class LwIWnd :
	public IWnd
{
public:
	LwIWnd();
	~LwIWnd();
	virtual bool Attch(int w);
	virtual void Detach();
	virtual int GetWindowId();
	virtual int GetPid();
	virtual RECT GetWindowRect();
	//键盘按键
	virtual bool KeyPress(int code);
	virtual bool KeyPressEx(int code, int state_key);
	//鼠标
	virtual bool MouseLeftClicked();
	virtual bool MouseLeftDoubleClicked();
	virtual bool MouseRightClicked();
	virtual bool MouseRightDoubleClicked();
	virtual bool MouseLeftDown();
	virtual bool MouseLeftUp();
	virtual bool MouseRightDown();
	virtual bool MouseRightUp();
	virtual bool MouseMoveTo(int x, int y);
	virtual bool MouseMiddleClicked();
	virtual bool MouseWheelDown();
	virtual bool MouseWheelUp();
	//找图找色找字
	virtual bool FindPicture(int x_1, int y_1, int x_2, int y_2, const char* pic_name,int *pos = nullptr,const char* delta_color = "000000", float sim = 0.95, int dir = 0);
	virtual vector<int> FindPictureEx(int x_1, int y_, int x_2, int y_2, const char* pic_name, const char* delta_color = "000000", float sim = 0.95, int dir = 0);
	virtual bool FindString(int x_1, int y_1, int x_2, int y_2, const char* string, const char* color, int* pos = nullptr, float sim = 0.95, int isbkc = 0);
	virtual bool FindColor(int x_1, int y_1, int x_2, int y_2, const char* color, int* pos = nullptr, float sim = 0.95, int dir = 0);
	virtual vector<int> FindColorEx(int x_1, int y_1, int x_2, int y_2, const char* color, float sim = 0.95, int dir = 0);
	//禁止外部输入
	virtual bool LockInput(int lock = 0);
private:
	int m_wid =-1;
	int m_pid =-1;
	RECT m_rect;
	CComPtr<Ilwsoft> pGetRes=nullptr;//智能指针
};

