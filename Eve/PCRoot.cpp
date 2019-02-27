#include "PCRoot.h"
#include "IconFont.h"
#include "PCEveClients.h"
#include <QString>
#include <QHostInfo>
#include <Windows.h>
#include <QListWidgetItem>
#include <QIcon>
#include <QFile>
#include <QDomDocument>
#include <QList>
#include <QUrl>
#include <QDataStream>
#include <QProcess>
#include <QProgressBar>
#include <QFileInfo>

PCRoot::PCRoot(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	bar = new QProgressBar(this);
	bar->setMaximum(0);
	bar->setMinimum(0);
	bar->setWindowFlag(Qt::Dialog);
	bar->setWindowModality(Qt::WindowModal);
	timer = new QTimer(this);
	bar->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	evereg = new EveRoleRegister(this);
	evereg->setWindowFlag(Qt::Dialog);
	evereg->setWindowModality(Qt::WindowModal);
	parse_role_list();
	init();
	connect(ui.pbRootReflash, SIGNAL(clicked()), this, SLOT(reflash_clicked()));
	connect(ui.pbAddition, SIGNAL(clicked()), this, SLOT(addition_clicked()));
	connect(ui.pbRemove, SIGNAL(clicked()), this, SLOT(remove_clicked()));
	connect(ui.pbAddition_a, SIGNAL(clicked()), this, SLOT(addition_all_clicked()));
	connect(ui.pbRemove_a, SIGNAL(clicked()), this, SLOT(remove_all_clicked()));
	connect(ui.pbPathset, SIGNAL(clicked()), this, SLOT(set_eve_launcher()));
	connect(ui.pbEve, SIGNAL(clicked()), this, SLOT(launch_eve()));
	connect(timer, SIGNAL(timeout()), this, SLOT(time_out()));
	connect(ui.listRoles, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));
	connect(evereg, SIGNAL(RegisterOk()), this, SLOT(register_ok()));
}

PCRoot::~PCRoot()
{
}

void PCRoot::addition_clicked()
{
	QList<QListWidgetItem *> items=ui.listRoles->selectedItems();
	for (int i = 0; i < items.size(); ++i)
	{
		QString role;
		role = items[i]->text().mid(6);
		if (PCEveClients::Instance()->GetRolesState(role) == PCEveClients::ClientStates::NotReady)
		{
			PCEveClients::Instance()->AlterEveRolesState(role,PCEveClients::ClientStates::Ready);
			items[i]->setTextColor(Qt::red);
		}
	}
}

void PCRoot::addition_all_clicked()
{
	for (int i = 0; i < m_lists.size(); ++i)
	{
		QString role;
		role = ui.listRoles->item(i)->text().mid(6);
		if (PCEveClients::Instance()->GetRolesState(role) == PCEveClients::ClientStates::NotReady)
		{
			PCEveClients::Instance()->AlterEveRolesState(role, PCEveClients::ClientStates::Ready);
			ui.listRoles->item(i)->setTextColor(Qt::red);
		}
	}
}

void PCRoot::remove_clicked()
{
	QList<QListWidgetItem *> items = ui.listRoles->selectedItems();
	for (int i = 0; i < items.size(); ++i)
	{
		QString role;
		role = items[i]->text().mid(6);
		if (PCEveClients::Instance()->GetRolesState(role) == PCEveClients::ClientStates::Ready)
		{
			PCEveClients::Instance()->AlterEveRolesState(role, PCEveClients::ClientStates::NotReady);
			items[i]->setTextColor(Qt::green);
		}
	}
}

void PCRoot::remove_all_clicked()
{
	for (int i = 0; i < m_lists.size(); ++i)
	{
		QString role;
		role = ui.listRoles->item(i)->text().mid(6);
		if (PCEveClients::Instance()->GetRolesState(role) == PCEveClients::ClientStates::Ready)
		{
			PCEveClients::Instance()->AlterEveRolesState(role, PCEveClients::ClientStates::NotReady);
			ui.listRoles->item(i)->setTextColor(Qt::green);
		}
	}
}

void PCRoot::set_eve_launcher()
{
	//QFileDialog filedialog(this, QString("EVE Launcher"), m_eveLuncherPath, ".exe");
	//filedialog.setViewMode(QFileDialog::List);
	//if (filedialog.exec() == QDialog::Accepted) {
	//	QUrl url;
	//	filedialog.selectUrl()
	//}
	QString file_path = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("请选择EVE启动路径..."), m_eveLuncherPath,QString::fromLocal8Bit("可执行文件 (*.exe)"));
	if (file_path.isEmpty())
	{
		return;
	}
	else
	{
		m_eveLuncherPath = file_path;
		QFile file("11111111.dat");
		file.open(QIODevice::Truncate | QIODevice::WriteOnly);
		QDataStream out(&file);
		out << m_eveLuncherPath;
	}
}

void PCRoot::launch_eve()
{
	QFileInfo fi = QFileInfo(m_eveLuncherPath);
	QString dir = fi.absolutePath();
	QProcess::startDetached(m_eveLuncherPath, QStringList(m_eveLuncherPath), dir);
	timeouts = 15;
	bar->show();
	timer->start(1000);
}

void PCRoot::time_out()
{
	--timeouts;
	QString title("EVE");
	const wchar_t * wtitle = reinterpret_cast<const wchar_t *>(title.utf16());
	HWND wid = FindWindow(__TEXT("Qt5QWindowIcon"), wtitle);
	if (wid != NULL)
	{
		ui.pbEve->setEnabled(false);
		bar->hide();
		timer->stop();
		return;
	}
	if (0 == timeouts)
	{
		bar->hide();
		timer->stop();
	}
}

void PCRoot::itemDoubleClicked(QListWidgetItem * item)
{
	QString role = item->text().mid(6);
	QStringList l;
	l.append(role);
	if (PCEveClients::Instance()->GetRolesState(role) == PCEveClients::ClientStates::Unauthorized)
	{
		evereg->SetRoleList(l);
		evereg->show();
	}
}

void PCRoot::register_ok()
{
	parse_role_list();
	reflash_clicked();
}

void PCRoot::init()
{
	QFont font = IconFont::Instance()->GetFont();
	font.setPointSize(18);
	ui.pcName->setFont(font);
	QString localHostName = QHostInfo::localHostName();
	ui.pcName->setText(QChar(0xf26c)+QString(" - ")+localHostName);
	ui.pcName->setStyleSheet("color:green;");
	ui.pbAddition->setFont(font);
	ui.pbAddition->setText(QChar(0xf105));
	ui.pbAddition->setStyleSheet("color:green;");
	ui.pbAddition_a->setFont(font);
	ui.pbAddition_a->setText(QChar(0xf101));
	ui.pbAddition_a->setStyleSheet("color:green;");
	ui.pbRemove->setFont(font);
	ui.pbRemove->setText(QChar(0xf104));
	ui.pbRemove->setStyleSheet("color:red;");
	ui.pbRemove_a->setFont(font);
	ui.pbRemove_a->setText(QChar(0xf100));
	ui.pbRemove_a->setStyleSheet("color:red;");
	ui.pbAddition->setFlat(true);
	ui.pbRemove->setFlat(true);
	ui.pbAddition_a->setFlat(true);
	ui.pbRemove_a->setFlat(true);
	ui.pbRootReflash->setFont(font);
	ui.pbRootReflash->setText(QChar(0xf021));
	ui.pbRootReflash->setStyleSheet("color:green;");

	QString title("EVE");
	const wchar_t * wtitle = reinterpret_cast<const wchar_t *>(title.utf16());
	HWND wid = FindWindow(__TEXT("Qt5QWindowIcon"), wtitle);
	if (wid != NULL)
	{
		ui.pbEve->setEnabled(false);
	}
	HWND hd = GetDesktopWindow();
	hd = GetWindow(hd, GW_CHILD);        //得到屏幕上第一个子窗口
	wchar_t s[200] = { 0 };
	QIcon icon_a;
	icon_a.addFile(QStringLiteral("pic/eve_on.jpg"));
	QIcon icon_u;
	icon_u.addFile(QStringLiteral("pic/eve.jpg"));
	while (hd != NULL)                    //循环得到所有的子窗口
	{
		memset(s, 0, 200);
		GetWindowText(hd, s, 200);
		QString str = QString::fromUtf16(reinterpret_cast<const char16_t *>(s));
		if (str.contains("EVE - ", Qt::CaseSensitive))
		{
			QString role = str.mid(6);
			if (is_authorized(role))
			{
				QListWidgetItem *item = new QListWidgetItem(icon_a, str);
				item->setTextColor(Qt::green);
				ui.listRoles->addItem(item);
				PCEveClients::Instance()->AddEveRoles(role, PCEveClients::ClientStates::NotReady);
			}
			else
			{
				QListWidgetItem *item = new QListWidgetItem(icon_u, str);
				item->setTextColor(Qt::gray);
				ui.listRoles->addItem(item);
				PCEveClients::Instance()->AddEveRoles(role, PCEveClients::ClientStates::Unauthorized);
			}
			m_lists.append(role);
		}
		hd = GetNextWindow(hd, GW_HWNDNEXT);
	}
	QFile file("11111111.dat");
	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);
	in >> m_eveLuncherPath;
}

void PCRoot::parse_role_list()
{
	m_authorized.clear();
	QFile file("setting/role.xml");
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		qDebug() << "Error: Cannot read file " << " setting/role.xml"
			<< ": " << qPrintable(file.errorString())
			<< endl;
		return;
	}
	QString errorStr;
	int errorLine;
	int errorColumn;
	QDomDocument doc;
	if (!doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn))
	{
		qDebug() << "Error: Parse error at line " << errorLine << ", "
			<< "column " << errorColumn << ": "
			<< qPrintable(errorStr) << endl;
		return;
	}
	QDomElement root = doc.documentElement();
	if (root.tagName() != "roles")
	{
		qDebug() << "Error setting file, setting/role.xml";
		return;
	}
	QDomNode node = root.firstChild();
	while (!node.isNull())
	{
		if (node.toElement().tagName() == "role")
		{
			QDomNode sub_node1 = node.toElement().firstChild();
			QDomNode sub_node2 = sub_node1.nextSibling();
			if (sub_node1.toElement().tagName() == "name" && sub_node2.toElement().tagName() == "scheme")
			{
				m_authorized.append(sub_node1.toElement().text());
			}
		}
		node = node.nextSibling();
	}
}

bool PCRoot::is_authorized(QString role)
{
	return m_authorized.contains(role, Qt::CaseInsensitive);
}

bool PCRoot::is_exist(QString role)
{
	return m_lists.contains(role);
}

void PCRoot::reflash_clicked()
{
	int num = ui.listRoles->count();
	for (int i = 0; i < num; ++i)
	{	
		QListWidgetItem *item=ui.listRoles->takeItem(0);
		delete item;
		item = nullptr;
	}
	m_lists.clear();
	HWND hd = GetDesktopWindow();
	hd = GetWindow(hd, GW_CHILD);        //得到屏幕上第一个子窗口
	wchar_t s[200] = { 0 };
	QIcon icon_a;
	icon_a.addFile(QStringLiteral("pic/eve_on.jpg"));
	QIcon icon_u;
	icon_u.addFile(QStringLiteral("pic/eve.jpg"));
	while (hd != NULL)                    //循环得到所有的子窗口
	{
		memset(s, 0, 200);
		GetWindowText(hd, s, 200);
		QString str = QString::fromUtf16(reinterpret_cast<const char16_t *>(s));
		if (str.contains("EVE - ", Qt::CaseSensitive))
		{
			QString role = str.mid(6);
			if (is_authorized(role))
			{
				QListWidgetItem *item = new QListWidgetItem(icon_a, str);
				ui.listRoles->addItem(item);
				if (PCEveClients::Instance()->GetRolesState(role) == PCEveClients::ClientStates::Undefined||
					PCEveClients::Instance()->GetRolesState(role) == PCEveClients::ClientStates::NotReady||
					PCEveClients::Instance()->GetRolesState(role) == PCEveClients::ClientStates::Unauthorized)
				{
					PCEveClients::Instance()->AddEveRoles(role, PCEveClients::ClientStates::NotReady);
					item->setTextColor(Qt::green);
				}
				else
				{
					PCEveClients::Instance()->AddEveRoles(role, PCEveClients::Instance()->GetRolesState(role));
					item->setTextColor(QColor("#ffff7f"));
				}
			}
			else
			{
				QListWidgetItem *item = new QListWidgetItem(icon_u, str);
				item->setTextColor(Qt::gray);
				ui.listRoles->addItem(item);
				PCEveClients::Instance()->AddEveRoles(role, PCEveClients::ClientStates::Unauthorized);
			}
			m_lists.append(role);
		}
		hd = GetNextWindow(hd, GW_HWNDNEXT);
	}
	QStringList roles = PCEveClients::Instance()->GetRoles();
	for (int i = 0; i < roles.size(); ++i)
	{
		if (!is_exist(roles[i]))
		{
			PCEveClients::Instance()->DelEveRoles(roles[i]);
		}
	}
}
