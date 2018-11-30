#include "Unitt.h"
#include "IWnd.h"
#include "LwIWnd.h"
#include "IScheme.h"
#include "Scheme_1.h"
#include <qDebug>
#include <QString>
#include <QPoint>
#include <vector>
#include <Windows.h>

namespace
{
	HWND GetWindowHwnd()
	{
		HWND hwnd = FindWindow(__T("triuiScreen"), __T("EVE - Jthree Robert"));
		if (hwnd == nullptr) {
			qDebug() << "没有找到窗口.";
		}
		else {
			qDebug() << "成功找到窗口.\n";
		}
		return hwnd;
	}
}

Unitt::Unitt(QObject* parent)
	:QObject(parent)
{

}


Unitt::~Unitt()
{
}

int Unitt::UttProc(int argc, char * argv[], int s)
{
	switch (s)
	{
	case 1:
		LwIWnd_integration_testing(); break;
	case 2:
		LwIWnd_KeyPressEx(); break;
	case 3:
		LwIWnd_FindPicture(); break;
	case 4:
		LwIWnd_FindString(); break;
	case 5:
		LwIWnd_FindColor(); break;
	case 6:
		RoleConfigure_Loadsettings(); break;
	case 7:
		RoleConfigure_GetOverviewPos(); break;
	case 8:
		Rolelist_ui();
	case 9:
		LwIWnd_FindColorEx();
	default:break;
	}
	return 0;
}

void Unitt::LwIWnd_integration_testing()
{
	HWND wid = GetWindowHwnd();
	IWnd *wnd_1 = new LwIWnd();
	wnd_1->Attch((int)wid);
	qDebug() << QString::fromLocal8Bit("窗口id： ")<<wnd_1->GetWindowId();
	qDebug() << wnd_1->GetPid();
	for (int i = 0; i < 100; ++i)
	{
		wnd_1->MouseMoveTo(18, 18);
		wnd_1->MouseLeftClicked();
		Sleep(200);
	}
	wnd_1->Detach();
	delete wnd_1;
	wnd_1 = nullptr;
}

void Unitt::LwIWnd_KeyPressEx()
{
	HWND wid = GetWindowHwnd();
	IWnd *wnd_1 = new LwIWnd();
	wnd_1->Attch((int)wid);
	qDebug() << QString::fromLocal8Bit("窗口id： ") << wnd_1->GetWindowId();
	qDebug() << wnd_1->GetPid();
	wnd_1->MouseMoveTo(537, 77);
	for (int i = 0; i < 10; ++i)
	{
		wnd_1->KeyPressEx('P', 1); //Alt+p
		wnd_1->MouseMoveTo(537, 77);
		Sleep(1000);
		wnd_1->MouseLeftClicked();
		Sleep(1000);
	}
	wnd_1->Detach();
}

void Unitt::LwIWnd_FindPicture()
{
	HWND wid = GetWindowHwnd();
	IWnd *wnd_1 = new LwIWnd();
	if(!wnd_1->Attch((int)wid))
	{
		qDebug() << QString::fromLocal8Bit("绑定窗口失败");
		return;
	}
	qDebug() << QString::fromLocal8Bit("窗口id： ") << wnd_1->GetWindowId();
	qDebug() << wnd_1->GetPid();
	qDebug() << QString::fromLocal8Bit("窗口大小: ") << wnd_1->GetWindowRect().right << " " << wnd_1->GetWindowRect().bottom;
	int pos[2] = {0,0};

	bool ret=wnd_1->FindPicture(0, 0, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom,"pic/EnterShipHangar_TakeControl_Undock.bmp",pos,"000000",0.7);
	qDebug() << QString::fromLocal8Bit("找图结果(pic/EnterShipHangar_TakeControl_Undock.bmp) ： ") << ret<<" x: "<<pos[0]<<" y: "<<pos[1];
	
	ret = wnd_1->FindPicture(0, 0, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom, "pic/local.bmp", pos, "000000", 0.7);
	qDebug() << QString::fromLocal8Bit("找图结果(pic/local.bmp) ： ") << ret << " x: " << pos[0] << " y: " << pos[1];

	ret = wnd_1->FindPicture(0, 0, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom, "pic/Overview_title.bmp", pos, "000000", 0.7);
	qDebug() << QString::fromLocal8Bit("找图结果(pic/Overview_title.bmp :)")<< ret << " x: " << pos[0] << " y: " << pos[1];

	ret = wnd_1->FindPicture(0, 0, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom, "pic/operating_panel.bmp", pos, "000000", 0.7);
	qDebug() << QString::fromLocal8Bit("找图结果(pic/operating_panel.bmp :)") << ret << " x: " << pos[0] << " y: " << pos[1];

	ret = wnd_1->FindPicture(0, 0, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom, "pic/select_item.bmp", pos, "000000", 0.7);
	qDebug() << QString::fromLocal8Bit("找图结果(pic/select_item.bmp :)") << ret << " x: " << pos[0] << " y: " << pos[1];

	wnd_1->Detach();
}

void Unitt::LwIWnd_FindString()
{
	HWND wid = GetWindowHwnd();
	IWnd *wnd_1 = new LwIWnd();
	wnd_1->Attch((int)wid);
	qDebug() << QString::fromLocal8Bit("窗口id： ") << wnd_1->GetWindowId();
	qDebug() << wnd_1->GetPid();
	qDebug() << QString::fromLocal8Bit("窗口大小: ") << wnd_1->GetWindowRect().right << " " << wnd_1->GetWindowRect().bottom;
	int pos[2] = { 0,0 };
	bool ret = wnd_1->FindString(0, 0, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom, "Undock", "E8E8E8-000000", pos,0.8);
	qDebug() << QString::fromLocal8Bit("找字结果 ： ") << ret << " x: " << pos[0] << " y: " << pos[1];
	wnd_1->Detach();
}

void Unitt::LwIWnd_FindColor()
{
	HWND wid = GetWindowHwnd();
	IWnd *wnd_1 = new LwIWnd();
	wnd_1->Attch((int)wid);
	qDebug() << QString::fromLocal8Bit("窗口id： ") << wnd_1->GetWindowId();
	qDebug() << wnd_1->GetPid();
	qDebug() << QString::fromLocal8Bit("窗口大小: ") << wnd_1->GetWindowRect().right << " " << wnd_1->GetWindowRect().bottom;
	int pos[2] = { 0,0 };
	bool ret = wnd_1->FindColor(0, 0, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom, "EE6666-000000", pos);
	qDebug() << QString::fromLocal8Bit("找色结果 ： ") << ret << " x: " << pos[0] << " y: " << pos[1];
	wnd_1->Detach();
}

void Unitt::RoleConfigure_Loadsettings()
{
	RoleConfigure c;
	c.Loadsettings("Jthree Robert");
	qDebug() << "Neocom Menu position: " << c.GetNeocomMenuPos();
	qDebug() << "Local position: " << c.GetLocalPos();
	qDebug() << "Instrument Panel position: " << c.GetInstrumentPanelPos();
	qDebug() << "Drones position: " << c.GetDronesPos();
	qDebug() << "Overview position: " << c.GetOverviewPos();
	qDebug() << "Select Item positon: " << c.GetSelectItemPos();
	qDebug() << "Undock positon " << c.GetUndockPos();
}

void Unitt::RoleConfigure_GetOverviewPos()
{
	RoleConfigure c;
	c.Loadsettings("Jthree Robert");
	HWND wid = GetWindowHwnd();
	IWnd *wnd_1 = new LwIWnd();
	wnd_1->Attch((int)wid);

	QPoint pt = c.GetOverviewPos("刷怪");
	qDebug() << QString::fromLocal8Bit("刷怪： ") << pt;
	wnd_1->MouseMoveTo(pt.x(), pt.y());
	wnd_1->MouseLeftClicked();
	Sleep(2000);
	
	pt = c.GetOverviewPos("空间");
	qDebug() << QString::fromLocal8Bit("空间： ") << pt;
	wnd_1->MouseMoveTo(pt.x(), pt.y());
	wnd_1->MouseLeftClicked();
	Sleep(2000);

	pt = c.GetOverviewPos("无人机");
	qDebug() << QString::fromLocal8Bit("无人机： ") << pt;
	wnd_1->MouseMoveTo(pt.x(), pt.y());
	wnd_1->MouseLeftClicked();
	Sleep(2000);

	pt = c.GetOverviewPos("残骸");
	qDebug() << QString::fromLocal8Bit("残骸： ") << pt;
	wnd_1->MouseMoveTo(pt.x(), pt.y());
	wnd_1->MouseLeftClicked();
	Sleep(2000);

	pt = c.GetOverviewPos("建筑");
	qDebug() << QString::fromLocal8Bit("建筑： ") << pt;
	wnd_1->MouseMoveTo(pt.x(), pt.y());
	wnd_1->MouseLeftClicked();
	Sleep(2000);

	pt = c.GetOverviewPos("跃迁");
	qDebug() << QString::fromLocal8Bit("跃迁： ") << pt;
	wnd_1->MouseMoveTo(pt.x(), pt.y());
	wnd_1->MouseLeftClicked();
	Sleep(2000);

	pt = c.GetOverviewPos("友军");
	qDebug() << QString::fromLocal8Bit("友军： ") << pt;
	wnd_1->MouseMoveTo(pt.x(), pt.y());
	wnd_1->MouseLeftClicked();
	Sleep(2000);
	wnd_1->Detach();
}

void Unitt::Rolelist_ui()
{
	Rolelist  *list = new Rolelist();
	list->show();
}

void Unitt::LwIWnd_FindColorEx()
{
	HWND wid = GetWindowHwnd();
	IWnd *wnd_1 = new LwIWnd();
	wnd_1->Attch((int)wid);
	qDebug() << QString::fromLocal8Bit("窗口id： ") << wnd_1->GetWindowId();
	qDebug() << wnd_1->GetPid();
	qDebug() << QString::fromLocal8Bit("窗口大小: ") << wnd_1->GetWindowRect().right << " " << wnd_1->GetWindowRect().bottom;
	int pos[2] = { 0,0 };
	std::string r = wnd_1->FindColorEx(769, 524, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom, "F80000-000000|F40000-000000|00F400-000000|00F800-000000|F4F400-000000|FFFF00-000000|E1E101-000000|F5F500-000000");
	if (r.size() < 5)
		qDebug()<<QString::fromLocal8Bit("无人机已回厂库");
	else
		qDebug()<<QString::fromLocal8Bit("无人机在空间中");
	wnd_1->Detach();
}
