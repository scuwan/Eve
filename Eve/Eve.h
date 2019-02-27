#pragma once

#include <QtWidgets/QWidget>
#include <QMainwindow>
#include <QThread>
#include <QDockWidget>
#include <QSharedPointer>
#include <QTime>
#include <QSlider>
#include "ui_Eve.h"
#include "OutputInfo.h"
#include "Rolelist.h"
#include "IScheme.h"
#include "GrabWindowAbnormalCondition.h"
#include "PCRoot.h"
#include <QMenu>
#include <QActionGroup>
class Eve : public QMainWindow
{
	Q_OBJECT

public:
	Eve(QMainWindow *parent = Q_NULLPTR);
	~Eve();
private slots:
	void launchRole(QString role, QString scheme,QTime t);
	void pauseRole(QString role);
	void restartRole(QString role);
	void stopRole(QString role);
	void releaseControl(QString role);
	void quit(IScheme*);
	void cmdBack(IScheme*, bool, int code);
	void shutDownTime(QString role, QTime t);
	void EveRoleState(QString name, int);
	void DelRole(QString name);
	void RoleOutputInfo(QString role, QString info,QString color);
	//Menu
	void windowOpacity();
	void sliderOpacityValue(int);
	void windowSwitch(QAction*);
private:
	bool is_role_launched(QString role);
	int role_index(QString role);
	int role_index_of_info(QString role);

private:
	Ui::MainWindow ui;
	Rolelist *rolesui;
	OutputInfo *cmdinfoui;
	PCRoot *pcrootui;
	QDockWidget *dockinfo;
	QSlider *sliderOpacity;
	QMenu * menuw;
	QVector<QAction*> wActions;
	QActionGroup *wGroup;
	struct RoleRuntime
	{
		RoleRuntime()
		{

		}
		~RoleRuntime()
		{
			if (thread != nullptr)
			{
				thread->quit();
				thread->wait();
				delete thread;
				thread = nullptr;
			}
			if (scheme != nullptr)
			{
				delete scheme;
				scheme = nullptr;
			}
		}
		QThread *thread = nullptr;
		IScheme *scheme = nullptr;
		QString role;
	};
	QVector<QSharedPointer<RoleRuntime>> m_runtime;
	struct RoleInfoWindow
	{
		QString role;
		QDockWidget* m_roleinfodock = nullptr;
		OutputInfo* m_roleinfoui =nullptr;
	};
	QVector<RoleInfoWindow> m_roleinfodocks;
	GrabWindowAbnormalCondition *m_grab = nullptr;
};
