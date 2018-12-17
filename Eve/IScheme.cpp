#include "IScheme.h"

QMap<QString, QSharedPointer<QMutex> > IScheme::m_gmutexs;

IScheme::IScheme(QString role, QString group)
{

}

IScheme::~IScheme()
{

}

QString IScheme::GetRole()
{
	return m_roleName;
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
