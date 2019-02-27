#pragma once
#include <QPoint>
#include <QString>
#include <QStringList>
class RoleConfigure
{
public:
	RoleConfigure();
	~RoleConfigure();
	bool Loadsettings(QString role);
	QPoint GetNeocomMenuPos();
	QPoint GetLocalPos();
	QPoint GetInstrumentPanelPos(QString key = QString("Base"));
	QPoint GetDronesPos(QString key=QString("Base"));
	QPoint GetOverviewPos(QString key=QString("Base"));
	QPoint GetOverviewFirstItem();
	QPoint GetSelectItemPos();
	QPoint GetUndockPos();
	QPoint GetStationItemPos(QString key = QString("Approach"));
	QPoint GetDronesLocalSpacePos(QString key = QString("Engage Target"));
	QPoint GetDroneBayPos(QString key = QString("Lanch drones"));
	QPoint GetAlliancePos();
	QPoint GetProbeScannerWraptoWithin_o(QString key = "Wrap to Within");
	int GetShiledArmour();
	QString GetWrapWithin();
private:
	 QPoint m_posNeocomMenu;
	 QPoint m_posLocal;
	 QPoint m_posInstrumentPanel;
	 QPoint m_posDrones;
	 QPoint m_posOverview;
	 QPoint m_posSelectItem;
	 QPoint m_posUndock;
	 QPoint m_posAlliance;
	 int m_shiledarmour;	//0- ¶Ü¿¹ 1- ¼×¿¹
	 QString m_wrapwithin;
};

