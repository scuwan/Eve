#pragma once
#include <string.h>
#include <istream>
#include <Windows.h>
#include <vector>
using std::string;
using std::vector;
class IWnd
{
public:
	IWnd() 
	{

	};
	~IWnd()
	{

	};
	virtual bool Attch(int w) = 0;
	virtual void Detach() = 0;
	virtual int GetWindowId() = 0;
	virtual int GetPid() = 0;
	virtual RECT GetWindowRect()=0;
	//º¸≈Ã∞¥º¸
	virtual bool KeyPress(int code) = 0;
	virtual bool KeyPressEx(int code, int state_key) = 0;
	// Û±Í
	virtual bool MouseLeftClicked() = 0;
	virtual bool MouseLeftDoubleClicked() = 0;
	virtual bool MouseRightClicked() = 0;
	virtual bool MouseRightDoubleClicked() = 0;
	virtual bool MouseLeftDown() = 0;
	virtual bool MouseLeftUp() = 0;
	virtual bool MouseRightDown() = 0;
	virtual bool MouseRightUp() = 0;
	virtual bool MouseMoveTo(int x, int y) = 0;
	virtual bool MouseMiddleClicked() = 0;
	virtual bool MouseWheelDown() = 0;
	virtual bool MouseWheelUp() = 0;
	//’“Õº’“…´’“◊÷
	virtual bool FindPicture(int x_1,int y_1,int x_2,int y_2,const char* pic_name,int* pos = nullptr,const char* delta_color="000000",float sim=0.95,int dir=0) = 0;
	virtual vector<int> FindPictureEx(int x_1, int y_1, int x_2, int y_2, const char* pic_name, const char* delta_color = "000000", float sim = 0.95, int dir = 0)=0;
	virtual bool FindString(int x_1, int y_1, int x_2, int y_2, const char* string, const char* color, int* pos=nullptr,float sim=0.95, int isbkc=0) = 0;
	virtual bool FindColor(int x_1, int y_1, int x_2, int y_2, const char* color, int* pos = nullptr, float sim = 0.95, int dir = 0) = 0;
	virtual vector<int> FindColorEx(int x_1, int y_1, int x_2, int y_2, const char* color, float sim = 0.95, int dir = 0) = 0;
	//Ω˚÷πÕ‚≤ø ‰»Î
	virtual bool LockInput(int lock = 0) = 0;	
};

