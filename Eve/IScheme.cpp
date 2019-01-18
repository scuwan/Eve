#include "IScheme.h"
#include <QDebug>

QMap<QString, QSharedPointer<QMutex> > IScheme::m_gmutexs;

IScheme::IScheme(QString role, QString group)
{
	m_shutdown =QDateTime(QDateTime::currentDateTime().date(),QTime(18, 10, 0, 0));
}

IScheme::~IScheme()
{

}

QString IScheme::GetRole()
{
	return m_roleName;
}

void IScheme::SetShutDownTime(QTime t)
{
	QDateTime dt= QDateTime::currentDateTime();
	if (dt.time() > t)
	{
		QDateTime dtt;
		dtt.setDate(dt.date());
		dtt=dtt.addDays(1);
		dtt.setTime(t);
		m_shutdown = dtt;
	}
	else
	{
		QDateTime dtt;
		dtt.setDate(dt.date());
		dtt.setTime(t);
		m_shutdown = dtt;
	}
	QString current_date = m_shutdown.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
	qDebug() << current_date;
}

void IScheme::Init()
{
	static int only_one = true;
	if (only_one)
	{
		only_one = false;
		m_gmutexs.insert(QString::fromLocal8Bit("Ä¬ÈÏ×é"), QSharedPointer<QMutex>(new QMutex()));
	}
}

void IScheme::Run()
{
	run();
}

void IScheme::output(const QString &info, const QString& color)
{
	QDateTime current_time = QDateTime::currentDateTime();
	QString current_date = current_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
	QString format_info = "[";
	format_info.append(current_date);
	format_info.append("] ");
	format_info.append(info);
	emit newInfo(format_info, color);
}

void IScheme::grab(int id,QString role)
{
	emit GrabWindowRequest(id, role);
}
