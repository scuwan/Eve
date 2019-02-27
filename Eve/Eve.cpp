#include "Eve.h"
#include "Scheme_1.h"
#include "PCEveClients.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QDockWidget>

Eve::Eve(QMainWindow *parent)
	: QMainWindow(parent)
{
	m_runtime.reserve(128);
	ui.setupUi(this);
	sliderOpacity = new QSlider(Qt::Horizontal,this);
	sliderOpacity->setMinimum(0);
	sliderOpacity->setMaximum(100);
	sliderOpacity->setSingleStep(1);
	sliderOpacity->setBaseSize(QSize(200, 40));
	sliderOpacity->setMaximumSize(200, 40);
	sliderOpacity->setMinimumSize(200, 40);
	sliderOpacity->setWindowFlags(windowFlags() ^ Qt::WindowMaximizeButtonHint | Qt::Dialog);
	sliderOpacity->setWindowIcon(QIcon(QString("pic/wicon.png")));
	sliderOpacity->hide();
	QIcon icon("pic/window.png");
	menuw = new QMenu(QString::fromLocal8Bit("窗口"),this);
	menuw->setIcon(icon);
	ui.menu->addMenu(menuw);
	wGroup = new QActionGroup(this);
	wGroup->setExclusive(false);
	m_grab = new GrabWindowAbnormalCondition(this);
	IScheme::Init();
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
	rolesui = new Rolelist(this);
	pcrootui = new PCRoot(this);
	splitter->addWidget(pcrootui);
	splitter->addWidget(rolesui);
	splitter->setStretchFactor(0, 1);
	splitter->setStretchFactor(1, 3);
	splitter->setOpaqueResize(true);
	//layout->addWidget(rolesui);
	layout->addWidget(splitter);
	ui.centralwidget->setLayout(layout);

	dockinfo = new  QDockWidget(QString::fromLocal8Bit("用户命令历史"), this);
	dockinfo->setFeatures(QDockWidget::NoDockWidgetFeatures);
	dockinfo->setAllowedAreas(Qt::BottomDockWidgetArea);
	cmdinfoui = new OutputInfo(dockinfo);
	dockinfo->setWidget(cmdinfoui);
	addDockWidget(Qt::BottomDockWidgetArea, dockinfo);

	QStringList ls = rolesui->Roles();
	for (int i = 0; i < ls.size(); ++i)
	{
		RoleInfoWindow iw;
		iw.role = ls[i];
		iw.m_roleinfodock = new  QDockWidget(iw.role, this);
		iw.m_roleinfodock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
		iw.m_roleinfodock->setAllowedAreas(Qt::BottomDockWidgetArea);
		iw.m_roleinfoui = new OutputInfo(iw.m_roleinfodock);
		iw.m_roleinfodock->setWidget(iw.m_roleinfoui);
		this->tabifyDockWidget(dockinfo, iw.m_roleinfodock);
		m_roleinfodocks.push_back(iw);
	}

	connect(rolesui, SIGNAL(launchRole(QString, QString,QTime)), this, SLOT(launchRole(QString, QString,QTime)));
	connect(rolesui, SIGNAL(pauseRole(QString)), this, SLOT(pauseRole(QString)));
	connect(rolesui, SIGNAL(restartRole(QString)), this, SLOT(restartRole(QString)));
	connect(rolesui, SIGNAL(stopRole(QString)), this, SLOT(stopRole(QString)));
	connect(rolesui, SIGNAL(releaseControl(QString)), this, SLOT(releaseControl(QString)));
	connect(rolesui, SIGNAL(cmdInfo(const QString&,const QString&)),cmdinfoui, SLOT(StandOut(const QString&,const QString&)));
	connect(rolesui, SIGNAL(shutDownTime(QString, QTime)), this, SLOT(shutDownTime(QString, QTime)));
	connect(PCEveClients::Instance(), SIGNAL(EveRoleState(QString, int)), this, SLOT(EveRoleState(QString, int)));
	connect(PCEveClients::Instance(), SIGNAL(DelRole(QString)), this, SLOT(DelRole(QString)));
	connect(rolesui, SIGNAL(RoleDetectedInfo(QString, QString, QString)), this, SLOT(RoleOutputInfo(QString, QString, QString)));
	//Menu
	connect(ui.actionwindowtrans, SIGNAL(triggered()), this, SLOT(windowOpacity()));
	connect(sliderOpacity, SIGNAL(valueChanged(int)), this, SLOT(sliderOpacityValue(int)));
	connect(wGroup, SIGNAL(triggered(QAction*)), this, SLOT(windowSwitch(QAction*)));
}

void Eve::launchRole(QString role, QString scheme,QTime time)
{
	if (is_role_launched(role))
		return;
	m_runtime.push_back(QSharedPointer<RoleRuntime>(new RoleRuntime()));
	m_runtime.last()->role = role;
	m_runtime.last()->thread = new QThread(this);
	m_runtime.last()->scheme = new Scheme_1(role);
	m_runtime.last()->scheme->SetShutDownTime(time);
	m_runtime.last()->scheme->moveToThread(m_runtime.last()->thread);
	connect(m_runtime.last()->scheme, SIGNAL(quit(IScheme*)), this, SLOT(quit(IScheme*)));
	connect(m_runtime.last()->scheme, SIGNAL(eState(IScheme*,bool,int)), this, SLOT(cmdBack(IScheme*,bool,int)));
	int index = role_index_of_info(role);
	connect(m_runtime.last()->scheme, SIGNAL(newInfo(const QString&,const QString&)), m_roleinfodocks[index].m_roleinfoui, SLOT(StandOut(const QString&,const QString&)));
	connect(m_runtime.last()->scheme, SIGNAL(GrabWindowRequest(const int, const QString)), m_grab, SLOT(GrabWindowRequest(const int, const QString)),Qt::QueuedConnection);
	m_runtime.last()->thread->start();
	rolesui->setThreadId(role,(int)m_runtime.last()->thread->currentThreadId());
	m_runtime.last()->scheme->Start();
}

Eve::~Eve()
{

}


void Eve::pauseRole(QString role)
{
	int index = role_index(role);
	if (index < 0)
		return;
	m_runtime[index]->scheme->SafePause();
}

void Eve::restartRole(QString role)
{
	int index = role_index(role);
	if (index < 0)
		return;
	m_runtime[index]->scheme->Start();
}

void Eve::stopRole(QString role)
{
	int index = role_index(role);
	if (index < 0)
		return;
	m_runtime[index]->scheme->SafeExit();
}

void Eve::releaseControl(QString role)
{
	int index = role_index(role);
	if (index < 0)
		return;
	m_runtime[index]->scheme->ReleaseControl();
}

void Eve::quit(IScheme *scheme)
{
	for (int i = 0; i < m_runtime.size(); ++i)
	{
		if (m_runtime[i]->role == scheme->GetRole())
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

void Eve::shutDownTime(QString role, QTime t)
{
	int index = role_index(role);
	if (index < 0)
		return;
	m_runtime[index]->scheme->SetShutDownTime(t);
}

void Eve::EveRoleState(QString name, int s)
{

	int index = role_index_of_info(name);
	if (-1 == index)
	{
		if (s == PCEveClients::ClientStates::Ready)
		{
			RoleInfoWindow iw;
			iw.role = name;
			iw.m_roleinfodock = new  QDockWidget(iw.role, this);
			iw.m_roleinfodock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
			iw.m_roleinfodock->setAllowedAreas(Qt::BottomDockWidgetArea);
			iw.m_roleinfoui = new OutputInfo(iw.m_roleinfodock);
			iw.m_roleinfodock->setWidget(iw.m_roleinfoui);
			this->tabifyDockWidget(dockinfo, iw.m_roleinfodock);
			m_roleinfodocks.push_back(iw);
			QIcon icon("pic/window.png");
			QAction * action = new QAction(icon, name, this);
			action->setCheckable(true);
			action->setChecked(true);
			menuw->addAction(action);
			wGroup->addAction(action);
			wActions.push_back(action);
		}
	}
	else
	{
		if (s == PCEveClients::ClientStates::NotReady)
		{
			QDockWidget *iw = m_roleinfodocks[index].m_roleinfodock;
			m_roleinfodocks.remove(index);
			iw->close();
			delete iw;
			iw = nullptr;

			for (int i = 0; i < wActions.size(); ++i)
			{
				if (wActions[i]->text() == name)
				{
					menuw->removeAction(wActions[i]);
					wGroup->removeAction(wActions[i]);
					delete wActions[i];
					wActions[i] = nullptr;
					wActions.remove(i);
					break;
				}
			}
		}
	}
}

void Eve::DelRole(QString name)
{
	int index = role_index_of_info(name);
	if (-1 != index)
	{
		QDockWidget *iw = m_roleinfodocks[index].m_roleinfodock;
		m_roleinfodocks.remove(index);
		iw->close();
		delete iw;
		iw = nullptr;

		for (int i = 0; i < wActions.size(); ++i)
		{
			if (wActions[i]->text() == name)
			{
				menuw->removeAction(wActions[i]);
				delete wActions[i];
				wActions[i] = nullptr;
				wActions.remove(i);
				break;
			}
		}
	}
}

void Eve::RoleOutputInfo(QString role, QString info,QString color)
{
	int index = role_index_of_info(role);
	m_roleinfodocks[index].m_roleinfoui->StandOut(info, color);
}

void Eve::windowOpacity()
{
	if (sliderOpacity->isHidden())
	{
		sliderOpacity->show();
	}
	else
	{
		sliderOpacity->hide();
	}
}

void Eve::sliderOpacityValue(int value)
{
	setWindowOpacity(value / 100.0);
}

void Eve::windowSwitch(QAction *action)
{
	QString role = action->text();
	int index = role_index_of_info(role);
	bool checked = action->isChecked();
	if (index != -1)
	{
		if (checked)
		{
			m_roleinfodocks[index].m_roleinfodock->show();
		}
		else
		{
			m_roleinfodocks[index].m_roleinfodock->hide();
		}
	}
}

bool Eve::is_role_launched(QString role)
{
	for (int i = 0; i < m_runtime.size(); ++i)
	{
		if (role == m_runtime[i]->role)
			return true;
	}
	return false;
}

int Eve::role_index(QString role)
{
	int index = -1;
	for (int i = 0; i < m_runtime.size(); ++i)
	{
		if (role == m_runtime[i]->role)
		{
			index = i;
			break;
		}
	}
	return index;
}

int Eve::role_index_of_info(QString role)
{
	int index = -1;
	for (int i = 0; i < m_roleinfodocks.size(); ++i)
	{
		if (role == m_roleinfodocks[i].role)
		{
			index = i;
			break;
		}
	}
	return index;
}
