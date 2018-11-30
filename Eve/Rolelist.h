#pragma once

#include <QWidget>
#include "ui_Rolelist.h"
#include <QDomElement>
#include <QDomDocument>
#include <QVector>
#include <QString>

class Rolelist : public QWidget
{
	Q_OBJECT

public:
	Rolelist(QWidget *parent = Q_NULLPTR);
	~Rolelist();
signals:
	void launchRole(QString role,QString scheme);
	void pauseRole(QString role);
	void restartRole(QString role);
	void stopRole(QString role);
	void releaseControl(QString role);
public:
	void setThreadId(QString role,int id);
public slots:
	void cmdExecStatus(QString role,bool ok,int code);
private slots:
	void cellClicked(int row, int column);
private:
	void parse_role_list();
	int role_index(QString role);
	int role_row(QString role);
private:
	Ui::Rolelist ui;
	struct Role
	{
		QString role;
		QString scheme;
		int threadId=0;
		int state=0;		/* 0-����,1-����,2-��ͣ*/
		int oldState = 0;
		int currentCmdcode = -1;	//0 - �ű��߳��˳� 1 - �ű��߳���ͣ 2 - �����ű��߳� 3 - �����ű��߳� 4 - �ͷſ���Ȩ
	};
	QVector<Role> m_roles;
};
