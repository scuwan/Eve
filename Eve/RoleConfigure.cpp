#include "RoleConfigure.h"
#include <QSettings>

namespace
{
	const int overview_o[][2] =
	{
		{ 16,34 },	/*刷怪*/
		{ 48,34 },	/*空间*/
		{ 80,34 },	/*无人机*/
		{ 112,34 },	/*残骸*/
		{ 144,34 },	/*建筑*/
		{ 176,34 },	/*跃迁*/
		{ 208,34 },	/*友军*/
		{ 80,70}	/*First Item*/
	};
	const int station_item_o[][2] =
	{
		{16,76},	/*Approach*/
		{41,76},	/*Wrap to with 0 m*/
		{66,76},	/*Dock*/
		{91,76},	/*Open Cargo Deposit*/
		{116,76},	/*Orbit*/
		{141,76},	/*Keep at Range(1,000m)*/
		{166,76},	/*Lock Target*/
		{191,76},	/*Lock at*/
		{216,103},	/*Track*/
	};
	const int drones_o[][2] = 
	{
		{30,35},	/*Drones in Bay*/
		{30,57}		/*Drones in Local Space*/
	};
	const int drones_in_local_space[][2] =
	{
		{15,8},		/*Engage Target*/
		{15,26},	/*Return and Orbit*/
		{15,42},	/*Return to Drone Bay*/
		{15,57},	/*Scoop to Drone Bay*/
		{15,76},	/*Expand*/
		{15,95}		/*Abandan Drones*/
	};
	const int drones_in_bay[][2] = 
	{
		{15,8}		/*Lanch drones*/
	};
	const int instrument_panel_o[][2]=
	{
		{10,27},	/*Stop The Ship*/	
		{103,27},	/*Set Full Speed*/
		{203,-7},	/*Equipment 1*/
		{178,38},	/*Equipemnt 2*/
		{228,38},	/*Equipemnt 3*/
		{255,-7}	/*Equipemnt 4*/
	};
	const int probe_scanner_wrap_to_within_o[][2]=
	{
		{57,23},	/*Wrap to Within*/
		{180,23},	/*Within 0 m*/
		{180,38},	/*Within 10 km*/
		{180,53},	/*Within 20 km*/
		{180,68},	/*Within 30 km*/
		{180,83},	/*Within 50 km*/
		{180,98},	/*Within 70 km*/
		{180,113},	/*Within 100 km*/
		{57,47}		/*Ignore Result*/
	};
}
RoleConfigure::RoleConfigure()
{

}


RoleConfigure::~RoleConfigure()
{
}

bool RoleConfigure::Loadsettings(QString role)
{
	QString setting =QString("./setting/")+role + "-Setting.ini";
	QSettings settings(setting, QSettings::IniFormat);
	settings.beginGroup("Window");
	QStringList l;
	l = settings.value("Neocom Menu","0 0").toString().split(" ");
	m_posNeocomMenu.setX(l[0].toInt());
	m_posNeocomMenu.setY(l[1].toInt());
	l = settings.value("Local").toString().split(" ");
	m_posLocal.setX(l[0].toInt());
	m_posLocal.setY(l[1].toInt());
	l = settings.value("Instrument Panel").toString().split(" ");
	m_posInstrumentPanel.setX(l[0].toInt());
	m_posInstrumentPanel.setY(l[1].toInt());
	l = settings.value("Drones").toString().split(" ");
	m_posDrones.setX(l[0].toInt());
	m_posDrones.setY(l[1].toInt());
	l = settings.value("Overview").toString().split(" ");
	m_posOverview.setX(l[0].toInt());
	m_posOverview.setY(l[1].toInt());
	l = settings.value("Select Item").toString().split(" ");
	m_posSelectItem.setX(l[0].toInt());
	m_posSelectItem.setY(l[1].toInt());
	l = settings.value("Undock").toString().split(" ");
	m_posUndock.setX(l[0].toInt());
	m_posUndock.setY(l[1].toInt());
	l = settings.value("Alliance").toString().split(" ");
	m_posAlliance.setX(l[0].toInt());
	m_posAlliance.setY(l[1].toInt());
	settings.endGroup();
	return true;
}

QPoint RoleConfigure::GetNeocomMenuPos()
{
	return m_posNeocomMenu;
}

QPoint RoleConfigure::GetLocalPos()
{
	return m_posLocal;
}

QPoint RoleConfigure::GetInstrumentPanelPos(QString key)
{
	//const int instrument_panel_o[][2] =
	//{
	//	{ 10,27 },	/*Stop The Ship*/
	//	{ 103,27 },	/*Set Full Speed*/
	//	{ 203,-7 },	/*Equipment 1*/
	//	{ 178,38 },	/*Equipemnt 2*/
	//	{ 228,38 }	/*Equipemnt 3*/
	//}
	if (key == "Base")
		return m_posInstrumentPanel;
	else if (key == "Stop The Ship")
	{
		QPoint pt(m_posInstrumentPanel.x() + instrument_panel_o[0][0], m_posInstrumentPanel.y() + instrument_panel_o[0][1]);
		return pt;
	}
	else if (key == "Set Full Speed")
	{
		QPoint pt(m_posInstrumentPanel.x() + instrument_panel_o[1][0], m_posInstrumentPanel.y() + instrument_panel_o[1][1]);
		return pt;
	}
	else if (key == "Equipment 1")
	{
		QPoint pt(m_posInstrumentPanel.x() + instrument_panel_o[2][0], m_posInstrumentPanel.y() + instrument_panel_o[2][1]);
		return pt;
	}
	else if (key == "Equipment 2")
	{
		QPoint pt(m_posInstrumentPanel.x() + instrument_panel_o[3][0], m_posInstrumentPanel.y() + instrument_panel_o[3][1]);
		return pt;
	}
	else if (key == "Equipment 3")
	{
		QPoint pt(m_posInstrumentPanel.x() + instrument_panel_o[4][0], m_posInstrumentPanel.y() + instrument_panel_o[4][1]);
		return pt;
	}
	else if (key == "Equipment 4")
	{
		QPoint pt(m_posInstrumentPanel.x() + instrument_panel_o[5][0], m_posInstrumentPanel.y() + instrument_panel_o[5][1]);
		return pt;
	}
	else
		return QPoint(0, 0);
}

QPoint RoleConfigure::GetDronesPos(QString key)
{
	//{30, 35},	/*Drones in Bay*/
	//{ 30,57 }		/*Drones in Local Space*/
	if (key == "Base")
		return m_posDrones;
	else if (key == "Drones in Bay")
	{
		QPoint pt(m_posDrones.x() + drones_o[0][0], m_posDrones.y() + drones_o[0][1]);
		return pt;
	}
	else if (key == "Drones in Local Space")
	{
		QPoint pt(m_posDrones.x() + drones_o[1][0], m_posDrones.y() + drones_o[1][1]);
		return pt;
	}
	else
		return QPoint(0, 0);
}

QPoint RoleConfigure::GetOverviewPos(QString key)
{
	if (key == "Base")
	{
		return m_posOverview;
	}
	else if (key == "刷怪")
	{
		QPoint pt(m_posOverview.x() + overview_o[0][0], m_posOverview.y() + overview_o[0][1]);
		return pt;
	}
	else if (key == "空间")
	{
		QPoint pt(m_posOverview.x() + overview_o[1][0], m_posOverview.y() + overview_o[1][1]);
		return pt;
	}
	else if (key == "无人机")
	{
		QPoint pt(m_posOverview.x() + overview_o[2][0], m_posOverview.y() + overview_o[2][1]);
		return pt;
	}
	else if (key == "残骸")
	{
		QPoint pt(m_posOverview.x() + overview_o[3][0], m_posOverview.y() + overview_o[3][1]);
		return pt;
	}
	else if (key == "建筑")
	{
		QPoint pt(m_posOverview.x() + overview_o[4][0], m_posOverview.y() + overview_o[4][1]);
		return pt;
	}
	else if (key == "跃迁")
	{
		QPoint pt(m_posOverview.x() + overview_o[5][0], m_posOverview.y() + overview_o[5][1]);
		return pt;
	}
	else if (key == "友军")
	{
		QPoint pt(m_posOverview.x() + overview_o[6][0], m_posOverview.y() + overview_o[6][1]);
		return pt;
	}
	else
		return QPoint(0, 0);
}

QPoint RoleConfigure::GetOverviewFirstItem()
{
	QPoint pt(m_posOverview.x() + overview_o[7][0], m_posOverview.y() + overview_o[7][1]);
	return pt;
}

QPoint RoleConfigure::GetSelectItemPos()
{
	return m_posSelectItem;
}

QPoint RoleConfigure::GetUndockPos()
{
	return m_posUndock;
}

QPoint RoleConfigure::GetStationItemPos(QString key)
{
	//{16, 76},		/*Approach*/
	//{ 41,76 },	/*Wrap to with 0 m*/
	//{ 66,76 },	/*Dock*/
	//{ 91,76 },	/*Open Cargo Deposit*/
	//{ 116,76 },	/*Orbit*/
	//{ 141,76 },	/*Keep at Range(1,000m)*/
	//{ 166,76 },	/*Lock Target*/
	//{ 191,76 },	/*Look at*/
	//{ 216,103 },	/*Track*/
	if (key == "Approach")
	{
		QPoint pt(m_posSelectItem.x() + station_item_o[0][0], m_posSelectItem.y() + station_item_o[0][1]);
		return pt;
	}
	else if (key == "Wrap to with 0 m")
	{
		QPoint pt(m_posSelectItem.x() + station_item_o[1][0], m_posSelectItem.y() + station_item_o[1][1]);
		return pt;
	}
	else if (key == "Dock")
	{
		QPoint pt(m_posSelectItem.x() + station_item_o[2][0], m_posSelectItem.y() + station_item_o[2][1]);
		return pt;
	}
	else if (key == "Open Cargo Deposit")
	{
		QPoint pt(m_posSelectItem.x() + station_item_o[3][0], m_posSelectItem.y() + station_item_o[3][1]);
		return pt;
	}
	else if (key == "Orbit")
	{
		QPoint pt(m_posSelectItem.x() + station_item_o[4][0], m_posSelectItem.y() + station_item_o[4][1]);
		return pt;
	}
	else if (key == "Keep at Range(1,000m)")
	{
		QPoint pt(m_posSelectItem.x() + station_item_o[5][0], m_posSelectItem.y() + station_item_o[5][1]);
		return pt;
	}
	else if (key == "Lock Target")
	{
		QPoint pt(m_posSelectItem.x() + station_item_o[6][0], m_posSelectItem.y() + station_item_o[6][1]);
		return pt;
	}
	else if (key == "Look at")
	{
		QPoint pt(m_posSelectItem.x() + station_item_o[7][0], m_posSelectItem.y() + station_item_o[7][1]);
		return pt;
	}
	else if (key == "Track")
	{
		QPoint pt(m_posSelectItem.x() + station_item_o[8][0], m_posSelectItem.y() + station_item_o[8][1]);
		return pt;
	}
	else
		return QPoint(0, 0);
}

QPoint RoleConfigure::GetDronesLocalSpacePos(QString key)
{
	//const int drones_in_local_space[][2] =
	//{
	//	{ 15,8 },		/*Engage Target*/
	//	{ 15,26 },	/*Return and Orbit*/
	//	{ 15,42 },	/*Return to Drone Bay*/
	//	{ 15,57 },	/*Scoop to Drone Bay*/
	//	{ 15,76 },	/*Expand*/
	//	{ 15,95 }		/*Abandan Drones*/
	//};
	if (key == "Engage Target")
	{
		QPoint pt(m_posDrones.x() + drones_o[1][0] + drones_in_local_space[0][0], m_posDrones.y() + drones_o[1][1] + drones_in_local_space[0][1]);
		return pt;
	}
	else if (key == "Return and Orbit")
	{
		QPoint pt(m_posDrones.x() + drones_o[1][0] + drones_in_local_space[1][0], m_posDrones.y() + drones_o[1][1] + drones_in_local_space[1][1]);
		return pt;
	}
	else if (key == "Return to Drone Bay")
	{
		QPoint pt(m_posDrones.x() + drones_o[1][0] + drones_in_local_space[2][0], m_posDrones.y() + drones_o[1][1] + drones_in_local_space[2][1]);
		return pt;
	}
	else if (key == "Scoop to Drone Bay")
	{
		QPoint pt(m_posDrones.x() + drones_o[1][0] + drones_in_local_space[3][0], m_posDrones.y() + drones_o[1][1] + drones_in_local_space[3][1]);
		return pt;
	}
	else if (key == "Expand")
	{
		QPoint pt(m_posDrones.x() + drones_o[1][0] + drones_in_local_space[4][0], m_posDrones.y() + drones_o[1][1] + drones_in_local_space[4][1]);
		return pt;
	}
	else if (key == "Abandan Drones")
	{
		QPoint pt(m_posDrones.x() + drones_o[1][0] + drones_in_local_space[5][0], m_posDrones.y() + drones_o[1][1] + drones_in_local_space[5][1]);
		return pt;
	}
	else
		return QPoint(0, 0);
}

QPoint RoleConfigure::GetDroneBayPos(QString key)
{
	if (key == "Lanch drones")
	{
		QPoint pt(m_posDrones.x() + drones_o[0][0] + drones_in_bay[0][0], m_posDrones.y() + drones_o[0][1] + drones_in_bay[0][1]);
		return pt;
	}
	else
		return QPoint(0, 0);
}

QPoint RoleConfigure::GetAlliancePos()
{
	return m_posAlliance;
}

QPoint RoleConfigure::GetProbeScannerWraptoWithin_o(QString key)
{
	//const int probe_scanner_wrap_to_within_o[][2] =
	//{
	//	{ 57,23 },	/*Wrap to Within*/
	//	{ 180,23 },	/*Within 0 m*/
	//	{ 180,38 },	/*Within 10 km*/
	//	{ 180,53 },	/*Within 20 km*/
	//	{ 180,68 },	/*Within 30 km*/
	//	{ 180,83 },	/*Within 50 km*/
	//	{ 180,98 },	/*Within 70 km*/
	//	{ 180,113 }	/*Within 100 km*/
	//  {57, 77}	/*Ignore Result*/
	//}
	if (key == "Wrap to Within")
	{
		QPoint pt(probe_scanner_wrap_to_within_o[0][0], probe_scanner_wrap_to_within_o[0][1]);
		return pt;
	}
	else if (key == "Within 0 m")
	{
		QPoint pt(probe_scanner_wrap_to_within_o[1][0], probe_scanner_wrap_to_within_o[1][1]);
		return pt;
	}
	else if (key == "Within 10 km")
	{
		QPoint pt(probe_scanner_wrap_to_within_o[2][0], probe_scanner_wrap_to_within_o[2][1]);
		return pt;
	}
	else if (key == "Within 20 km")
	{
		QPoint pt(probe_scanner_wrap_to_within_o[3][0], probe_scanner_wrap_to_within_o[3][1]);
		return pt;
	}
	else if (key == "Within 30 km")
	{
		QPoint pt(probe_scanner_wrap_to_within_o[4][0], probe_scanner_wrap_to_within_o[4][1]);
		return pt;
	}
	else if (key == "Within 50 km")
	{
		QPoint pt(probe_scanner_wrap_to_within_o[5][0], probe_scanner_wrap_to_within_o[5][1]);
		return pt;
	}
	else if (key == "Within 70 km")
	{
		QPoint pt(probe_scanner_wrap_to_within_o[6][0], probe_scanner_wrap_to_within_o[6][1]);
		return pt;
	}
	else if (key == "Within 100 km")
	{
		QPoint pt(probe_scanner_wrap_to_within_o[7][0], probe_scanner_wrap_to_within_o[7][1]);
		return pt;
	}
	else if (key == "Ignore Result")
	{
		QPoint pt(probe_scanner_wrap_to_within_o[8][0], probe_scanner_wrap_to_within_o[8][1]);
		return pt;
	}
	else
		return QPoint(0,0);
}
