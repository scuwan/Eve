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
			qDebug() << "û���ҵ�����.";
		}
		else {
			qDebug() << "�ɹ��ҵ�����.\n";
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
	qDebug() << QString::fromLocal8Bit("����id�� ")<<wnd_1->GetWindowId();
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
	qDebug() << QString::fromLocal8Bit("����id�� ") << wnd_1->GetWindowId();
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
		qDebug() << QString::fromLocal8Bit("�󶨴���ʧ��");
		return;
	}
	qDebug() << QString::fromLocal8Bit("����id�� ") << wnd_1->GetWindowId();
	qDebug() << wnd_1->GetPid();
	qDebug() << QString::fromLocal8Bit("���ڴ�С: ") << wnd_1->GetWindowRect().right << " " << wnd_1->GetWindowRect().bottom;
	int pos[2] = {0,0};

	bool ret=wnd_1->FindPicture(0, 0, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom,"pic/EnterShipHangar_TakeControl_Undock.bmp",pos,"000000",0.7);
	qDebug() << QString::fromLocal8Bit("��ͼ���(pic/EnterShipHangar_TakeControl_Undock.bmp) �� ") << ret<<" x: "<<pos[0]<<" y: "<<pos[1];
	
	ret = wnd_1->FindPicture(0, 0, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom, "pic/local.bmp", pos, "000000", 0.7);
	qDebug() << QString::fromLocal8Bit("��ͼ���(pic/local.bmp) �� ") << ret << " x: " << pos[0] << " y: " << pos[1];

	ret = wnd_1->FindPicture(0, 0, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom, "pic/Overview_title.bmp", pos, "000000", 0.7);
	qDebug() << QString::fromLocal8Bit("��ͼ���(pic/Overview_title.bmp :)")<< ret << " x: " << pos[0] << " y: " << pos[1];

	ret = wnd_1->FindPicture(0, 0, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom, "pic/operating_panel.bmp", pos, "000000", 0.7);
	qDebug() << QString::fromLocal8Bit("��ͼ���(pic/operating_panel.bmp :)") << ret << " x: " << pos[0] << " y: " << pos[1];

	ret = wnd_1->FindPicture(0, 0, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom, "pic/select_item.bmp", pos, "000000", 0.7);
	qDebug() << QString::fromLocal8Bit("��ͼ���(pic/select_item.bmp :)") << ret << " x: " << pos[0] << " y: " << pos[1];

	wnd_1->Detach();
}

void Unitt::LwIWnd_FindString()
{
	HWND wid = GetWindowHwnd();
	IWnd *wnd_1 = new LwIWnd();
	wnd_1->Attch((int)wid);
	qDebug() << QString::fromLocal8Bit("����id�� ") << wnd_1->GetWindowId();
	qDebug() << wnd_1->GetPid();
	qDebug() << QString::fromLocal8Bit("���ڴ�С: ") << wnd_1->GetWindowRect().right << " " << wnd_1->GetWindowRect().bottom;
	int pos[2] = { 0,0 };
	bool ret = wnd_1->FindString(0, 0, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom, "Undock", "E8E8E8-000000", pos,0.8);
	qDebug() << QString::fromLocal8Bit("���ֽ�� �� ") << ret << " x: " << pos[0] << " y: " << pos[1];
	wnd_1->Detach();
}

void Unitt::LwIWnd_FindColor()
{
	HWND wid = GetWindowHwnd();
	IWnd *wnd_1 = new LwIWnd();
	wnd_1->Attch((int)wid);
	qDebug() << QString::fromLocal8Bit("����id�� ") << wnd_1->GetWindowId();
	qDebug() << wnd_1->GetPid();
	qDebug() << QString::fromLocal8Bit("���ڴ�С: ") << wnd_1->GetWindowRect().right << " " << wnd_1->GetWindowRect().bottom;
	int pos[2] = { 0,0 };
	bool ret = wnd_1->FindColor(0, 0, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom, "EE6666-000000", pos);
	qDebug() << QString::fromLocal8Bit("��ɫ��� �� ") << ret << " x: " << pos[0] << " y: " << pos[1];
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

	QPoint pt = c.GetOverviewPos("ˢ��");
	qDebug() << QString::fromLocal8Bit("ˢ�֣� ") << pt;
	wnd_1->MouseMoveTo(pt.x(), pt.y());
	wnd_1->MouseLeftClicked();
	Sleep(2000);
	
	pt = c.GetOverviewPos("�ռ�");
	qDebug() << QString::fromLocal8Bit("�ռ䣺 ") << pt;
	wnd_1->MouseMoveTo(pt.x(), pt.y());
	wnd_1->MouseLeftClicked();
	Sleep(2000);

	pt = c.GetOverviewPos("���˻�");
	qDebug() << QString::fromLocal8Bit("���˻��� ") << pt;
	wnd_1->MouseMoveTo(pt.x(), pt.y());
	wnd_1->MouseLeftClicked();
	Sleep(2000);

	pt = c.GetOverviewPos("�к�");
	qDebug() << QString::fromLocal8Bit("�к��� ") << pt;
	wnd_1->MouseMoveTo(pt.x(), pt.y());
	wnd_1->MouseLeftClicked();
	Sleep(2000);

	pt = c.GetOverviewPos("����");
	qDebug() << QString::fromLocal8Bit("������ ") << pt;
	wnd_1->MouseMoveTo(pt.x(), pt.y());
	wnd_1->MouseLeftClicked();
	Sleep(2000);

	pt = c.GetOverviewPos("ԾǨ");
	qDebug() << QString::fromLocal8Bit("ԾǨ�� ") << pt;
	wnd_1->MouseMoveTo(pt.x(), pt.y());
	wnd_1->MouseLeftClicked();
	Sleep(2000);

	pt = c.GetOverviewPos("�Ѿ�");
	qDebug() << QString::fromLocal8Bit("�Ѿ��� ") << pt;
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
	qDebug() << QString::fromLocal8Bit("����id�� ") << wnd_1->GetWindowId();
	qDebug() << wnd_1->GetPid();
	qDebug() << QString::fromLocal8Bit("���ڴ�С: ") << wnd_1->GetWindowRect().right << " " << wnd_1->GetWindowRect().bottom;
	int pos[2] = { 0,0 };
	std::string r = wnd_1->FindColorEx(769, 524, wnd_1->GetWindowRect().right, wnd_1->GetWindowRect().bottom, "F80000-000000|F40000-000000|00F400-000000|00F800-000000|F4F400-000000|FFFF00-000000|E1E101-000000|F5F500-000000");
	if (r.size() < 5)
		qDebug()<<QString::fromLocal8Bit("���˻��ѻس���");
	else
		qDebug()<<QString::fromLocal8Bit("���˻��ڿռ���");
	wnd_1->Detach();
}
