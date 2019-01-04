#pragma once

#include <QWidget>
#include "ui_Rolelist.h"
#include <QDomElement>
#include <QDomDocument>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QTime>

class Rolelist : public QWidget
{
	Q_OBJECT

public:
	Rolelist(QWidget *parent = Q_NULLPTR);
	~Rolelist();
	QStringList Roles();
signals:
	void launchRole(QString role,QString scheme);
	void pauseRole(QString role);
	void restartRole(QString role);
	void stopRole(QString role);
	void releaseControl(QString role);
	void cmdInfo(const QString& info,const QString& color);
	void shutDownTime(QString role, QTime);
public:
	void setThreadId(QString role,int id);
public slots:
	void cmdExecStatus(QString role,bool ok,int code);
private slots:
	void cellClicked(int row, int column);
	void timeChanged(const QTime & time);
private:
	void parse_role_list();
	int role_index(QString role);
	int role_row(QString role);
	void print_info(const QString& info,QString role=QString::fromLocal8Bit("全局"),const QString color="#ffffff");
private:
	Ui::Rolelist ui;
	struct Role
	{
		QString role;
		QString scheme;
		int threadId=0;
		int state=0;		/* 0-离线,1-在线,2-暂停*/
		int oldState = 0;
		int currentCmdcode = -1;	//0 - 脚本线程退出 1 - 脚本线程暂停 2 - 启动脚本线程 3 - 重启脚本线程 4 - 释放控制权
		QTime shutDown;
	};
	QVector<Role> m_roles;
};
