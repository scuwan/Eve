#include "Eve.h"
#include "Scheme_1.h"
#include <QSplitter>
#include <QVBoxLayout>

Eve::Eve(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QVBoxLayout *layout = new QVBoxLayout(this);
	QSplitter *splitter = new QSplitter(this);
	splitter->setOrientation(Qt::Vertical);
	rolesui = new Rolelist(splitter);
	outputui = new OutputInfo(splitter);
	splitter->addWidget(rolesui);
	splitter->addWidget(outputui);
	splitter->setStretchFactor(0, 4);
	splitter->setStretchFactor(1, 1);
	layout->addWidget(splitter);
	connect(rolesui, SIGNAL(launchRole(QString, QString)), this, SLOT(launchRole(QString, QString)));
	connect(rolesui, SIGNAL(pauseRole(QString)), this, SLOT(pauseRole(QString)));
	connect(rolesui, SIGNAL(restartRole(QString)), this, SLOT(restartRole(QString)));
	connect(rolesui, SIGNAL(stopRole(QString)), this, SLOT(stopRole(QString)));
	connect(rolesui, SIGNAL(releaseControl(QString)), this, SLOT(releaseControl(QString)));
}

void Eve::launchRole(QString role, QString scheme)
{
	if (is_role_launched(role))
		return;
	m_runtime.push_back(RoleRuntime());
	m_runtime.last().role = role;
	m_runtime.last().thread = new QThread(this);
	m_runtime.last().scheme = new Scheme_1(role);
	m_runtime.last().scheme->moveToThread(m_runtime.last().thread);
	connect(m_runtime.last().scheme, SIGNAL(quit(IScheme*)), this, SLOT(quit(IScheme*)));
	connect(m_runtime.last().scheme, SIGNAL(newInfo(IScheme*)), outputui, SLOT(Monitor(IScheme*)));
	connect(m_runtime.last().scheme, SIGNAL(eState(IScheme*,bool,int)), this, SLOT(cmdBack(IScheme*,bool,int)));
	m_runtime.last().thread->start();
	rolesui->setThreadId(role,(int)m_runtime.last().thread->currentThreadId());
	m_runtime.last().scheme->Start();
}

Eve::~Eve()
{

}

void Eve::pauseRole(QString role)
{
	int index = role_index(role);
	if (index < 0)
		return;
	m_runtime[index].scheme->SafePause();
}

void Eve::restartRole(QString role)
{
	int index = role_index(role);
	if (index < 0)
		return;
	m_runtime[index].scheme->Start();
}

void Eve::stopRole(QString role)
{
	int index = role_index(role);
	if (index < 0)
		return;
	m_runtime[index].scheme->SafeExit();
}

void Eve::releaseControl(QString role)
{
	int index = role_index(role);
	if (index < 0)
		return;
	m_runtime[index].scheme->ReleaseControl();
}

void Eve::quit(IScheme *scheme)
{
	for (int i = 0; i < m_runtime.size(); ++i)
	{
		if (m_runtime[i].role == scheme->GetRole())
		{
			m_runtime.remove(i);
			break;
		}
	}
}

void Eve::cmdBack(IScheme *scheme, bool ok, int code)
{
	rolesui->cmdExecStatus(scheme->GetRole(), ok, code);
	if (code == 2)
		rolesui->cmdExecStatus(scheme->GetRole(), ok, 3);
}

bool Eve::is_role_launched(QString role)
{
	for (int i = 0; i < m_runtime.size(); ++i)
	{
		if (role == m_runtime[i].role)
			return true;
	}
	return false;
}

int Eve::role_index(QString role)
{
	int index = -1;
	for (int i = 0; i < m_runtime.size(); ++i)
	{
		if (role == m_runtime[i].role)
		{
			index = i;
			break;
		}
	}
	return index;
}
