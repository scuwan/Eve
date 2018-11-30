#pragma once
#include "IWnd.h"
#include "IScheme.h"
#include "Scheme_1.h"
#include "RoleConfigure.h"
#include <QObject>
#include <Rolelist.h>
class Unitt:public QObject
{
	Q_OBJECT
public:
	Unitt(QObject* parent=nullptr);
	~Unitt();
	int UttProc(int argc, char* argv[],int s);
	void LwIWnd_integration_testing();  //s=1
	void LwIWnd_KeyPressEx();	//s=2
	void LwIWnd_FindPicture();	//s=3
	void LwIWnd_FindString();	//s=4
	void LwIWnd_FindColor();	//s=5
	void RoleConfigure_Loadsettings();		//s=6
	void RoleConfigure_GetOverviewPos();	//s=7
	void Rolelist_ui();	//s=8
	void LwIWnd_FindColorEx();	//s=9
};
