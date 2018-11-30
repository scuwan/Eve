#pragma once

#include <QtWidgets/QWidget>
#include <QThread>
#include "ui_Eve.h"
#include "OutputInfo.h"
#include "Rolelist.h"
#include "IScheme.h"
class Eve : public QWidget
{
	Q_OBJECT

public:
	Eve(QWidget *parent = Q_NULLPTR);
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

private:
	Ui::EveClass ui;
	Rolelist *rolesui;
	OutputInfo *outputui;
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
	QVector<RoleRuntime> m_runtime;
};
