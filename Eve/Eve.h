#pragma once

#include <QtWidgets/QWidget>
#include <QMainwindow>
#include <QThread>
#include <QDockWidget>
#include <QSharedPointer>
#include "ui_Eve.h"
#include "OutputInfo.h"
#include "Rolelist.h"
#include "IScheme.h"
#include "GrabWindowAbnormalCondition.h"
class Eve : public QMainWindow
{
	Q_OBJECT

public:
	Eve(QMainWindow *parent = Q_NULLPTR);
	~Eve();
private slots:
	void launchRole(QString role, QString scheme);
	void pauseRole(QString role);
	void restartRole(QString role);
	void stopRole(QString role);
	void releaseControl(QString role);
	void quit(IScheme*);
	void cmdBack(IScheme*, bool, int code);
private:
	bool is_role_launched(QString role);
	int role_index(QString role);
	int role_index_of_info(QString role);

private:
	Ui::MainWindow ui;
	Rolelist *rolesui;
	OutputInfo *cmdinfoui;
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
