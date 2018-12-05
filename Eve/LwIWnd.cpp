#include "LwIWnd.h"



LwIWnd::LwIWnd()
{
	CoInitialize(NULL);
}


LwIWnd::~LwIWnd()
{
	CoUninitialize();
}

bool LwIWnd::Attch(int w)
{
	CLSID clsid;
	HRESULT is_ok = CLSIDFromProgID(OLESTR("lw.lwsoft3"), &clsid);
	is_ok = pGetRes.CoCreateInstance(clsid);
	if (is_ok)
		return false;
	if (0 == pGetRes->BindWindow(w, 4, 3, 3, 0, 0))
	{
		pGetRes.Release();
		return false;
	}
	m_wid = w;
	DWORD pid;
	GetWindowThreadProcessId((HWND)w, &pid);
	m_pid = pid;
	RECT rect;
	bool ret = GetClientRect((HWND)m_wid, &rect);
	if (!ret) {
		m_rect.right = 0;
		m_rect.left = 0;
		m_rect.top = 0;
		m_rect.bottom = 0;
	}
	else
		m_rect = rect;
	return true;
}

void LwIWnd::Detach()
{
	pGetRes->UnBindWindow();
	pGetRes.Release();//小心哦!!请看最后的“注意”
}

int LwIWnd::GetWindowId()
{
	return m_wid;
}

int LwIWnd::GetPid()
{
	return m_pid;
}

RECT LwIWnd::GetWindowRect()
{
	return m_rect;
}

bool LwIWnd::KeyPress(int code)
{
	if (pGetRes != nullptr)
		return pGetRes->KeyPress(code,1);
	else
		return false;
}

bool LwIWnd::KeyPressEx(int code, int state_key)
{
	if (pGetRes != nullptr)
		return pGetRes->KeyPressEx(code, state_key);
	else
		return false;
}

bool LwIWnd::MouseLeftClicked()
{
	if (pGetRes != nullptr)
		return pGetRes->LeftClick();
	else
		return false;
}

bool LwIWnd::MouseLeftDoubleClicked()
{
	if (pGetRes != nullptr)
		return pGetRes->LeftDoubleClick();
	else
		return false;
}

bool LwIWnd::MouseRightClicked()
{
	if (pGetRes != nullptr)
		return pGetRes->RightClick();
	else
		return false;
}

bool LwIWnd::MouseRightDoubleClicked()
{
	if (pGetRes != nullptr)
	{
		pGetRes->RightClick();
		return pGetRes->RightClick();
	}
	else
		return false;
}

bool LwIWnd::MouseLeftDown()
{
	if (pGetRes != nullptr)
		return pGetRes->LeftDown();
	else
		return false;
}

bool LwIWnd::MouseLeftUp()
{
	if (pGetRes != nullptr)
		return pGetRes->LeftUp();
	else
		return false;
}

bool LwIWnd::MouseRightDown()
{
	if (pGetRes != nullptr)
		return pGetRes->RightDown();
	else
		return false;
}

bool LwIWnd::MouseRightUp()
{
	if (pGetRes != nullptr)
		return pGetRes->RightUp();
	else
		return false;
}

bool LwIWnd::MouseMoveTo(int x, int y)
{
	if (pGetRes != nullptr)
		return pGetRes->MoveTo(x, y);
	else
		return false;
}

bool LwIWnd::MouseMiddleClicked()
{
	if (pGetRes != nullptr)
		return pGetRes->MiddleClick();
	else
		return false;
}

bool LwIWnd::MouseWheelDown()
{
	if (pGetRes != nullptr)
		return pGetRes->WheelDown();
	else
		return false;
}

bool LwIWnd::MouseWheelUp()
{
	if (pGetRes != nullptr)
		return pGetRes->WheelUp();
	else
		return false;
}

bool LwIWnd::FindPicture(int x_1, int y_1, int x_2, int y_2, const char * pic_name, int *pos,const char * delta_color, float sim, int dir)
{
	if (pGetRes != nullptr)
	{
		bool ret =pGetRes->FindPic(x_1, y_1, x_2, y_2, _bstr_t(pic_name), _bstr_t(delta_color), sim, dir, 0, 0, 0, 0, 0);
		if (ret&&pos!=nullptr)
		{
			pos[0]=pGetRes->x();
			pos[1] = pGetRes->y();
		}
		return ret;
	}
	else
		return false;
}

string LwIWnd::FindPictureEx(int x_1, int y_1, int x_2, int y_2, const char * pic_name, const char * delta_color, float sim, int dir)
{
	if (pGetRes != nullptr)
	{
		BSTR str = pGetRes->FindPicEx(x_1, y_1, x_2, y_2, _bstr_t(pic_name), _bstr_t(delta_color), sim, dir);
		if (str == nullptr)
			return string();
		else
		{
			string s = _bstr_t(str);
			SysFreeString(str);
			return s;
		}
	}
	else
		return string();
}

bool LwIWnd::FindString(int x_1, int y_1, int x_2, int y_2, const char * string, const char * color, int * pos, float sim, int isbkc)
{
	if (pGetRes != nullptr)
	{
		pGetRes->UseDict(1);
		bool ret = pGetRes->FindStr(x_1, y_1, x_2, y_2, _bstr_t(string), _bstr_t(color),sim, isbkc);
		if (ret&&pos != nullptr)
		{
			pos[0] = pGetRes->x();
			pos[1] = pGetRes->y();
		}
		return ret;
	}
	return false;
}

bool LwIWnd::FindColor(int x_1, int y_1, int x_2, int y_2, const char * color, int * pos, float sim, int dir)
{
	if (pGetRes != nullptr)
	{
		bool ret=pGetRes->FindColor(x_1, y_1, x_2, y_2, _bstr_t(color), sim, dir,0,0,0,0,0);
		if (ret && pos != nullptr)
		{
			pos[0] = pGetRes->x();
			pos[1] = pGetRes->y();
		}
		return ret;
	}
	return false;
}

string LwIWnd::FindColorEx(int x_1, int y_1, int x_2, int y_2, const char * color, float sim, int dir)
{
	if (pGetRes != nullptr)
	{
		BSTR str = pGetRes->FindColorEx(x_1, y_1, x_2, y_2, _bstr_t(color), sim, dir);
		if (str == nullptr)
			return string();
		else
		{
			string s = _bstr_t(str);
			SysFreeString(str);
			return s;
		}
	}
	else
		return string();
}

bool LwIWnd::LockInput(int lock)
{
	if (pGetRes != nullptr)
		return pGetRes->LockInput(lock);
	else
		return false;
}
