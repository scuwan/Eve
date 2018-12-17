#pragma once
#include <QObject>
#include <QVector>
#include <QMutex>
#include <QMutexLocker>
#include <QDatetime>
#include <QSharedPointer>
#include <QMap>
class IScheme :public QObject
{
	Q_OBJECT
public:
	IScheme(QString role,QString group= QString::fromLocal8Bit("默认组"));
	virtual ~IScheme();
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
	QString GetRole();
	static void Init();
private:
	virtual void run() = 0;
signals:
	void eState(IScheme*, bool, int code);	    //执行状态
	void newInfo(const QString& info,const QString& color="#ffffff");			//新的消息
	void quit(IScheme*);
	void GrabWindowRequest(const int wid, const QString role);
public slots:
	virtual void Run();
protected:
	void output(const QString &info, const QString& color = "#ffffff");
	void grab(int id,QString role);
protected:
	QString m_roleName = "";
	QString m_groupName = QString::fromLocal8Bit("默认组");
	const QString DGROUPNAME = QString::fromLocal8Bit("默认组");
	static QMap<QString, QSharedPointer<QMutex> > m_gmutexs;
private:
	QVector<QString> m_info;
	int flag = 1;
};
