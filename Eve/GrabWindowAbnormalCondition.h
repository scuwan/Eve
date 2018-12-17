#pragma once

#include <QObject>
#include<QThread>
#include<QTimer>
#include<QMutex>
#include<QMap>

class GrabExecute;
class GrabWindowAbnormalCondition : public QObject
{
	Q_OBJECT

public:
	GrabWindowAbnormalCondition(QObject *parent);
	~GrabWindowAbnormalCondition();
public slots:
	void GrabWindowRequest(const int wid, const QString role);
signals:
	void grab(const int wid, const QString role);
private:
	QThread *thread;
	GrabExecute *m_grab;
	
};

class GrabExecute :public QObject
{
	Q_OBJECT
public:
	GrabExecute(QObject *parent=nullptr);
	~GrabExecute();
public slots:
	void GrabWindowRequest(const int wid, const QString role);
	void timeout();

private:
	struct  RoleDir
	{
		RoleDir(const QString& r, const QString& p)
			:role(r),dir(p)
		{

		}
		~RoleDir()
		{

		}
		QString role;
		QString dir;
	};
	QTimer *m_timer=nullptr;
	int m_tcnt = 0;
	QMutex m_mutex;
	QMap<int, RoleDir> m_wids;
	GrabWindowAbnormalCondition *m_grab=nullptr;
};