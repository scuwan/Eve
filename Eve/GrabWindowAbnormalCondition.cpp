#include "GrabWindowAbnormalCondition.h"
#include <QMutexLocker>
#include <QDir>
#include <QApplication>
#include <QTime>
#include <QPixmap>
#include <QDebug>
#include <QScreen>

GrabWindowAbnormalCondition::GrabWindowAbnormalCondition(QObject *parent)
	: QObject(parent)
{
	thread = new QThread();
	m_grab = new GrabExecute();
	m_grab->moveToThread(thread);
	thread->start();
	connect(this, SIGNAL(grab(const int, const QString)), m_grab, SLOT(GrabWindowRequest(const int, const QString)));
}

GrabWindowAbnormalCondition::~GrabWindowAbnormalCondition()
{
	thread->quit();
	thread->wait();
	if (m_grab != nullptr)
		delete m_grab;
}

void GrabWindowAbnormalCondition::GrabWindowRequest(const int wid, const QString role)
{
	emit grab(wid, role);
}

GrabExecute::GrabExecute(QObject *parent)
	:QObject(parent)
{
	connect(this, SIGNAL(stop_timer()), this, SLOT(StopTimer()));
}

GrabExecute::~GrabExecute()
{
	emit stop_timer();
	QThread::sleep(1);
}

void GrabExecute::timeout()
{
	static int index= 0;
	--m_tcnt;
	if (m_tcnt == 0)
	{
		m_wids.clear();
		m_timer->stop();
	}
	QMap<int, RoleDir>::const_iterator t = m_wids.cbegin();
	QDateTime current_time = QDateTime::currentDateTime();
	QString current_date = current_time.toString("yyyy MM dd hh mm ss zzz");
	QScreen *screen = QGuiApplication::primaryScreen();
	for (; t != m_wids.cend(); ++t)
	{
		QString file = t.value().dir+"/";
		file.append(current_date);
		file.append(".jpg");
		QPixmap p= screen->grabWindow(t.key());
		screen->grabWindow(t.key(), 0, 0, -1, -1).save(file,"JPG");
	}
}

void GrabExecute::StopTimer()
{
	if (m_timer != nullptr)
	{
		m_timer->stop();
		delete m_timer;
	}
}

void GrabExecute::GrabWindowRequest(const int wid, const QString role)
{
	if (1)
	{
		QMutexLocker t(&m_mutex);
		if (m_timer == nullptr)
		{
			m_timer = new QTimer(this);
			connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
		}
	}
	QDir dir;
	QString path = qApp->applicationDirPath();
	path.append("/grab/");
	path.append(role);
	bool ret = true;
	if (!dir.exists(path))
	{
		if (!dir.mkpath(path))
		{
			ret = false;
			qDebug() << QString::fromLocal8Bit("创建文件夹失败 ") << path;
		}
	}
	if (!ret)
	{
		path = "./grab";
	}
	
	if (m_wids.constFind(wid) == m_wids.constEnd())
	{
		QDateTime current_time = QDateTime::currentDateTime();
		QString current_date = current_time.toString("yyyy.MM.dd hh.mm.ss.zzz ddd");
		path.append("/");
		path.append(current_date);
		if (!dir.mkpath(path))
		{
			ret = false;
			qDebug() << QString::fromLocal8Bit("创建文件夹失败 ") << path;
		}
		else
			m_wids.insert(wid, RoleDir(role,path));
	}
	m_tcnt = 600;
	if (!m_timer->isActive())
	{
		m_timer->start(500);	//500 ms 抓屏
	}
}
