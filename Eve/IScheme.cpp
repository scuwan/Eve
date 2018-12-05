#include "IScheme.h"

QMap<QString, QSharedPointer<QMutex> > IScheme::m_gmutexs;

IScheme::IScheme(QString role, QString group)
{

}

IScheme::~IScheme()
{

}

void IScheme::GetInfo(QVector<QString>& i)
{
	i.clear();
	QMutexLocker locker(&m_mutex);
	i.swap(m_info);
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
