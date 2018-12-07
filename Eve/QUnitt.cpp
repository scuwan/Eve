#include "QUnitt.h"
#include "Scheme_1.h"

#include <QDebug>

QUnitt::QUnitt(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_scheme = new Scheme_1("Jthree Robert");
	m_scheme->moveToThread(&m_thread);
	connect(&m_thread, SIGNAL(started()), m_scheme, SLOT(Run()));
	connect(m_scheme, SIGNAL(quit()), &m_thread, SLOT(quit()));
	connect(m_scheme, SIGNAL(quit()), this, SLOT(quit()));
	connect(m_scheme, SIGNAL(newInfo(IScheme*)), this, SLOT(print_info(IScheme*)));
	connect(ui.p1, SIGNAL(clicked()), this, SLOT(safe_pause()));
	m_thread.start();
}

QUnitt::~QUnitt()
{
}

void QUnitt::print_info(IScheme* scheme)
{
	/*QVector<QString> info;
	scheme->GetInfo(info);
	for (int i = 0; i < info.size(); ++i)
	{
		qDebug() << info[i];
	}*/
}

void QUnitt::quit()
{
	qDebug() << "quit";
}

void QUnitt::safe_pause()
{
	m_scheme->SafePause();
}
