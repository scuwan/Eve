#pragma once
#include <QObject>
#include <QVector>
#include <QMutex>
#include <QMutexLocker>
#include <QDatetime>
class IScheme:public QObject
{
	Q_OBJECT
public:
	IScheme(QString role)
	{

	}
	virtual ~IScheme()
	{

	}
	/* code =0*/
	virtual void SafeExit() = 0;
	/* code =1*/
	virtual void SafePause() = 0;
	/* code =2*/
	virtual void Start() = 0;
	/* code =3*/
	virtual void ReleaseControl() = 0;
	virtual int GetLastError() = 0;
	virtual QString SchemeName() = 0;
	QString GetRole()
	{
		return m_roleName;
	}
	void GetInfo(QVector<QString>& i)
	{
		i.clear();
		QMutexLocker locker(&m_mutex);
		i.swap(m_info);
	}
private:
	virtual void run()=0;
signals:
	void eState(IScheme*,bool,int code);	    //执行状态
	void newInfo(IScheme*);			//新的消息
	void quit(IScheme*);
public slots:
	virtual void Run()
	{
		run();
	}
protected:
	void output(QString &info)
	{
		QDateTime current_time = QDateTime::currentDateTime();
		QString current_date = current_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
		QString format_info = "[";
		format_info.append(current_date);
		format_info.append("] ");
		format_info.append(info);
		QMutexLocker locker(&m_mutex);
		m_info.push_back(format_info);
		emit newInfo(this);
	}
protected:
	QString m_roleName = "";
private:
	QVector<QString> m_info;
	int flag = 1;
	QMutex m_mutex;
};

