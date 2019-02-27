#include "EveRoleRegister.h"
#include <QListWidgetItem>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QDomText>
#include <QTextStream>
#include <QMessageBox>
EveRoleRegister::EveRoleRegister(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.listWidget->setViewMode(QListView::IconMode);
	ui.listWidget->setIconSize(QSize(50, 50));
	connect(ui.pbSubmit, SIGNAL(clicked()), this, SLOT(submit()));
}

EveRoleRegister::~EveRoleRegister()
{
}

void EveRoleRegister::SetRoleList(QStringList & roles)
{
	int num = ui.listWidget->count();
	for (int i = 0; i < num; ++i)
	{
		QListWidgetItem *item = ui.listWidget->takeItem(0);
		delete item;
		item = nullptr;
	}
	for (int i = 0; i < roles.size(); ++i)
	{
		QListWidgetItem *item = new QListWidgetItem(QIcon("pic/eve.jpg"), roles[i]);
		ui.listWidget->addItem(item);
	}
}

void EveRoleRegister::submit()
{
	QFile file("setting/role.xml");
	if (!file.open(QFile::ReadOnly))
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("注册失败."));
		return;
	}
	QDomDocument doc;
	if (!doc.setContent(&file))
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("注册失败."));
		file.close();
		return;
	}
	file.close();
	for (int i = 0; i < ui.listWidget->count(); ++i)
	{
		QListWidgetItem *item = ui.listWidget->item(i);
		QString file = "setting/" + item->text() + QString("-Setting.ini");
		QFile::copy("setting/template.ini", file);

		QDomElement root = doc.documentElement();
		QDomElement role = doc.createElement("role");
		QDomElement name = doc.createElement("name");
		QDomElement scheme = doc.createElement("scheme");
		QDomText textn;
		textn = doc.createTextNode(item->text());
		QDomText texts;
		texts = doc.createTextNode(QString::fromLocal8Bit("方案1"));
		name.appendChild(textn);
		scheme.appendChild(texts);
		role.appendChild(name);
		role.appendChild(scheme);
		root.appendChild(role);
	}
	if (!file.open(QFile::WriteOnly | QFile::Truncate))
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("注册失败."));
		return;
	}
	QTextStream out_stream(&file);
	doc.save(out_stream, 4);
	file.flush();
	file.close();
	emit RegisterOk();
	QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("注册成功."));
	int num = ui.listWidget->count();
	for (int i = 0; i < num; ++i)
	{
		QListWidgetItem *item = ui.listWidget->takeItem(0);
		delete item;
		item = nullptr;
	}
	this->hide();
}
