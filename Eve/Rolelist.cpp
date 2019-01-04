#include "Rolelist.h"
#include <QFile>
#include <QDebug>
#include <QTableWidgetItem>
#include <QDateTime>
#include <IconFont.h>
#include <QDateTimeEdit>

Rolelist::Rolelist(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	parse_role_list();
	connect(ui.tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(cellClicked(int, int)));
}

Rolelist::~Rolelist()
{
}

QStringList Rolelist::Roles()
{
	QStringList s;
	for (int i = 0; i < m_roles.size(); ++i)
	{
		s.append(m_roles[i].role);
	}
	return s;
}

void Rolelist::setThreadId(QString role,int id)
{
	int rows = ui.tableWidget->rowCount();
	for (int i = 0; i < rows; ++i)
	{
		if (ui.tableWidget->item(i, 0)->text() == role)
		{
			ui.tableWidget->item(i, 6)->setText(QString::number(id));
		}
	}
}

void Rolelist::cmdExecStatus(QString role,bool ok, int code)
{
	int index = role_index(role);
	if (index < 0)
		return;
	int row = role_row(role);
	if (row < 0)
		return;
	switch (code)
	{
	case 0:	//Exit
	{
		//ui.tableWidget->item(row, column - 2)->setTextColor(Qt::green);
		//ui.tableWidget->item(row, column-1)->setText(QChar(0xf04c) + QString::fromLocal8Bit(" ��ͣ"));	/* ��ͣͼ��*/
		//ui.tableWidget->item(row, column-1)->setTextColor(Qt::gray);
		//ui.tableWidget->item(row, column)->setTextColor(Qt::gray);
		//ui.tableWidget->item(row, column + 1)->setText(QString::fromLocal8Bit("����"));
		//ui.tableWidget->item(row, column + 1)->setTextColor(Qt::gray);
		//m_roles[row].state = 0;
		if (ok)
		{
			ui.tableWidget->item(row, 2)->setTextColor(Qt::green);
			ui.tableWidget->item(row,3)->setText(QChar(0xf04c));
			ui.tableWidget->item(row, 3)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, 4)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, 5)->setText(QString::fromLocal8Bit("�ű�����!!!"));
			ui.tableWidget->item(row, 5)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, 6)->setText("N/A");
			ui.tableWidget->item(row, 7)->setText(QChar(0xf127));	/*unlink ͼ��*/
			ui.tableWidget->item(row, 7)->setTextColor(Qt::red);
			m_roles[index].state = 0;
			print_info(QString::fromLocal8Bit("�ű��߳��˳��ɹ�!!!"), role);
		}
		else
		{
			ui.tableWidget->item(row, 4)->setTextColor(Qt::green);
			m_roles[index].state = m_roles[index].oldState;
		}
	}
		break;
	case 1:	//��ͣ
	{
		/*ui.tableWidget->item(row, column-1)->setTextColor(Qt::gray);
		ui.tableWidget->item(row, column)->setText(QChar(0xf04c) + QString::fromLocal8Bit(" ����"));
		ui.tableWidget->item(row, column)->setTextColor(Qt::yellow);
		ui.tableWidget->item(row, column+1)->setTextColor(Qt::green);
		ui.tableWidget->item(row, column + 2)->setText(QString::fromLocal8Bit("��ͣ"));
		ui.tableWidget->item(row, column + 2)->setTextColor(Qt::yellow);
		m_roles[row].state = 2;*/
		if (ok&&m_roles[index].oldState==1)
		{
			ui.tableWidget->item(row, 2)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, 3)->setText(QChar(0xf04c));
			ui.tableWidget->item(row, 3)->setTextColor(Qt::green);
			ui.tableWidget->item(row, 4)->setTextColor(Qt::green);
			ui.tableWidget->item(row, 5)->setText(QString::fromLocal8Bit("�ű��߳�����ִͣ��"));
			ui.tableWidget->item(row, 5)->setTextColor(Qt::yellow);
			ui.tableWidget->item(row, 7)->setText(QChar(0xf127));	/*unlink ͼ��*/
			ui.tableWidget->item(row, 7)->setTextColor(Qt::red);
			m_roles[row].state = 2;
			print_info(QString::fromLocal8Bit("�ű��߳���ͣ�ɹ�!!!"), role);
		}
		else
		{
			ui.tableWidget->item(row, 3)->setTextColor(Qt::green);
			m_roles[row].state = 1;
		}
	}
		break;
	case 2:	//����
	{
		/*ui.tableWidget->item(row, column + 3)->setText(QString::fromLocal8Bit("����"));
		ui.tableWidget->item(row, column + 3)->setTextColor(Qt::green);
		ui.tableWidget->item(row, column)->setTextColor(Qt::gray);
		ui.tableWidget->item(row, column + 1)->setTextColor(Qt::green);
		ui.tableWidget->item(row, column + 2)->setTextColor(Qt::green);
		m_roles[row].state = 1;*/
		if (ok)
		{	
			if (m_roles[index].oldState == 0)
			{
				ui.tableWidget->item(row, 5)->setText(QString::fromLocal8Bit("����ˢ��"));
				ui.tableWidget->item(row, 5)->setTextColor(Qt::green);
				ui.tableWidget->item(row, 2)->setTextColor(Qt::gray);
				ui.tableWidget->item(row, 3)->setTextColor(Qt::green);
				ui.tableWidget->item(row, 4)->setTextColor(Qt::green);
				ui.tableWidget->item(row, 7)->setText(QChar(0xf0c1));	/*unlink ͼ��*/
				ui.tableWidget->item(row, 7)->setTextColor(Qt::green);
				m_roles[index].state = 1;	/* ����״̬ */
				m_roles[index].oldState = -2;
				print_info(QString::fromLocal8Bit("�ű��߳������ɹ�!!!"), role);
			}
		}
		else
		{
			ui.tableWidget->item(row, 5)->setText(QString::fromLocal8Bit("ˢ���߳�����ʧ��"));
			ui.tableWidget->item(row, 5)->setTextColor(Qt::red);
			ui.tableWidget->item(row, 2)->setTextColor(Qt::green);
			ui.tableWidget->item(row, 3)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, 4)->setTextColor(Qt::gray);
			m_roles[index].state = 0;	/* ����״̬ */
			m_roles[index].oldState = -2;
			print_info(QString::fromLocal8Bit("�ű��߳�����ʧ��!!!"), role);
		}
	}
		break;
	case 3:	//����
	{
		//ui.tableWidget->item(row, column - 1)->setTextColor(Qt::gray);
		//ui.tableWidget->item(row,column)->setText(QChar(0xf04c) + QString::fromLocal8Bit(" ��ͣ"));	/* ��ͣͼ��*/
		//ui.tableWidget->item(row, column)->setTextColor(Qt::green);
		//ui.tableWidget->item(row, column + 1)->setTextColor(Qt::green);
		//ui.tableWidget->item(row, column + 2)->setText(QString::fromLocal8Bit("����"));
		//ui.tableWidget->item(row, column + 2)->setTextColor(Qt::green);
		if (ok)
		{
			if (m_roles[index].oldState == 2)
			{
				ui.tableWidget->item(row, 2)->setTextColor(Qt::gray);
				ui.tableWidget->item(row, 3)->setText(QChar(0xf04c));
				ui.tableWidget->item(row, 3)->setTextColor(Qt::green);
				ui.tableWidget->item(row, 4)->setTextColor(Qt::green);
				ui.tableWidget->item(row, 5)->setText(QString::fromLocal8Bit("����ˢ��"));
				ui.tableWidget->item(row, 5)->setTextColor(Qt::green);
				ui.tableWidget->item(row, 7)->setText(QChar(0xf0c1));	/*unlink ͼ��*/
				ui.tableWidget->item(row, 7)->setTextColor(Qt::green);
				m_roles[index].state = 1;
				m_roles[index].oldState = -2;
				print_info(QString::fromLocal8Bit("�ű��߳������ɹ�!!!"), role);
			}
		}
		else
		{
			ui.tableWidget->item(row, 3)->setTextColor(Qt::green);
			m_roles[index].state = 2;
			m_roles[index].oldState = -2;
			print_info(QString::fromLocal8Bit("�ű��߳�����ʧ��!!!"), role);
		}
	}
	case 4: //�ͷſ���Ȩ
	{
		if (ok&&m_roles[index].oldState == 1)
		{
			ui.tableWidget->item(row, 3)->setText(QChar(0xf04c));	/*����ͼ��*/
			ui.tableWidget->item(row, 3)->setTextColor(Qt::green);
			ui.tableWidget->item(row, 4)->setTextColor(Qt::green);
			ui.tableWidget->item(row, 5)->setText(QString::fromLocal8Bit("�ű��߳�����ִͣ��"));
			ui.tableWidget->item(row, 5)->setTextColor(Qt::yellow);
			ui.tableWidget->item(row, 7)->setText(QChar(0xf127));	/*unlink ͼ��*/
			ui.tableWidget->item(row, 7)->setTextColor(Qt::red);
			m_roles[index].state = 2;
			m_roles[index].oldState = -2;
			print_info(QString::fromLocal8Bit("�ű��߳��ͷſ���Ȩ�ɹ�!!!"), role);
		}
	}
		break;
	default:
		break;
	}
}

void Rolelist::timeChanged(const QTime & time)
{
	for (int i = 0; i < m_roles.size(); ++i)
	{
		if ((static_cast<QTimeEdit*>(ui.tableWidget->cellWidget(i, 8)))->time() != m_roles[i].shutDown)
		{
			m_roles[i].shutDown = (static_cast<QTimeEdit*>(ui.tableWidget->cellWidget(i, 8)))->time();
			emit shutDownTime(m_roles[i].role, m_roles[i].shutDown);
		}
	}
}

void Rolelist::parse_role_list()
{
	QFile file("setting/role.xml");
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		qDebug() << "Error: Cannot read file " << " setting/role.xml"
			<< ": " << qPrintable(file.errorString())
			<<endl;
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
				Role role;
				role.role = sub_node1.toElement().text();
				role.scheme = sub_node2.toElement().text();
				role.state = 0;
				m_roles.push_back(role);
			}
		}
		node = node.nextSibling();
	}

	ui.tableWidget->setRowCount(m_roles.size());
	for (int i = 0; i < m_roles.size(); ++i)
	{
		QTableWidgetItem *Item = new QTableWidgetItem();
		Item->setText(m_roles[i].role);
		Item->setFlags(Item->flags() ^ Qt::ItemIsEditable);
		ui.tableWidget->setItem(i, 0, Item);
		Item = new QTableWidgetItem();
		Item->setText(m_roles[i].scheme);
		ui.tableWidget->setItem(i, 1, Item);
		QFont font = IconFont::Instance()->GetFont();
		Item = new QTableWidgetItem();
		Item->setFont(font);
		Item->setText(QChar(0xf04b));	/* ����ͼ�� */
		Item->setFlags(Item->flags() ^ Qt::ItemIsEditable);
		Item->setTextColor(Qt::green);
		Item->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget->setItem(i, 2, Item);
		Item = new QTableWidgetItem();
		Item->setFont(font);
		Item->setText(QChar(0xf04c));	/* ��ͣͼ��*/
		Item->setTextColor(Qt::gray);
		Item->setTextAlignment(Qt::AlignCenter);
		Item->setFlags(Item->flags() ^ Qt::ItemIsEditable);
		ui.tableWidget->setItem(i, 3, Item);
		Item = new QTableWidgetItem();
		Item->setFont(font);
		Item->setText(QChar(0xf04d));	/* ֹͣͼ��*/
		Item->setTextColor(Qt::gray);
		Item->setTextAlignment(Qt::AlignCenter);
		Item->setFlags(Item->flags() ^ Qt::ItemIsEditable);
		ui.tableWidget->setItem(i, 4, Item);
		Item = new QTableWidgetItem();
		Item->setTextAlignment(Qt::AlignCenter);
		Item->setText(QString::fromLocal8Bit("����"));
		Item->setFlags(Item->flags() ^ Qt::ItemIsEditable);
		ui.tableWidget->setItem(i, 5, Item);
		Item = new QTableWidgetItem();
		Item->setTextAlignment(Qt::AlignCenter);
		Item->setText(QString::fromLocal8Bit("NA"));
		Item->setFlags(Item->flags() ^ Qt::ItemIsEditable);
		ui.tableWidget->setItem(i, 6, Item);
		Item = new QTableWidgetItem();
		Item->setFont(font);
		Item->setTextAlignment(Qt::AlignCenter);
		Item->setText(QChar(0xf127));	/* un link ͼ��*/
		Item->setTextColor(Qt::red);
		Item->setFlags(Item->flags() ^ Qt::ItemIsEditable);
		ui.tableWidget->setItem(i, 7, Item);
		QTimeEdit * time = new QTimeEdit(this);
		time->setTime(QTime(18, 10, 0, 0));
		ui.tableWidget->setCellWidget(i, 8, time);
		connect(time, SIGNAL(timeChanged(const QTime &)), this, SLOT(timeChanged(const QTime &)));
		m_roles[i].shutDown = QTime(18, 10, 0, 0);
	}
}

int Rolelist::role_index(QString role)
{
	int index = -1;
	for (int i = 0; i < m_roles.size(); ++i)
	{
		if (role == m_roles[i].role)
		{
			index = i;
			break;
		}
	}
	return index;
}

int Rolelist::role_row(QString role)
{
	int row = -1;
	for (int i = 0; i < ui.tableWidget->rowCount(); ++i)
	{
		if (role == ui.tableWidget->item(i, 0)->text())
		{
			row = i;
		}
	}
	return row;
}

void Rolelist::print_info(const QString & info,QString role, const QString color)
{
	QDateTime current_time = QDateTime::currentDateTime();
	QString current_date = current_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
	QString format_info = "[";
	format_info.append(current_date);
	format_info.append("] ");
	format_info.append("[");
	format_info.append(role);
	format_info.append("] ");
	format_info.append(info);
	emit cmdInfo(format_info, color);
}

void Rolelist::cellClicked(int row, int column)
{
	QString role = ui.tableWidget->item(row, 0)->text();
	switch (column)
	{
	case 2:
	{
		if (m_roles[row].state == 0)	//����
		{
			ui.tableWidget->item(row, column + 1)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, column + 2)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, column)->setTextColor(Qt::yellow);
			ui.tableWidget->item(row, column + 3)->setText(QString::fromLocal8Bit("�ű��߳�������..."));
			ui.tableWidget->item(row, column + 3)->setTextColor(Qt::yellow);
			m_roles[row].oldState = 0;
			m_roles[row].currentCmdcode = 2;
			print_info(QString::fromLocal8Bit("�����ű��߳�..."), role,"#ff0000");
			emit launchRole(m_roles[row].role, m_roles[row].scheme,m_roles[row].shutDown);
		}
		break;
	}
	case 3:
	{
		if (m_roles[row].state == 1)	//��ͣ
		{
			ui.tableWidget->item(row, column - 1)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, column + 1)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, column)->setTextColor(Qt::yellow);
			ui.tableWidget->item(row, column + 2)->setText(QString::fromLocal8Bit("�ű��߳���ͣ��..."));
			ui.tableWidget->item(row, column + 2)->setTextColor(Qt::yellow);
			m_roles[row].oldState = 1;
			m_roles[row].currentCmdcode = 1;
			m_roles[row].state = -2;	//����ִ����
			print_info(QString::fromLocal8Bit("��ͣ�ű��߳�..."), role,"#ff0000");
			emit pauseRole(m_roles[row].role);
		}
		else if (m_roles[row].state == 2)	//����
		{
			ui.tableWidget->item(row, column - 1)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, column + 1)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, column)->setTextColor(Qt::yellow);
			ui.tableWidget->item(row, column + 2)->setText(QString::fromLocal8Bit("�ű��߳�������..."));
			ui.tableWidget->item(row, column + 2)->setTextColor(Qt::yellow);
			m_roles[row].currentCmdcode = 3;
			m_roles[row].oldState = 2;
			m_roles[row].state = -2;	//����ִ����
			print_info(QString::fromLocal8Bit("�����ű��߳�..."), role,"#ff0000");
			emit restartRole(m_roles[row].role);
		}
		break;
	}
	case 4:
	{
		if (m_roles[row].state == 1||m_roles[row].state==2)	//�˳�
		{
			ui.tableWidget->item(row, column - 2)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, column - 1)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, column)->setTextColor(Qt::yellow);
			ui.tableWidget->item(row, 7)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, column + 1)->setText(QString::fromLocal8Bit("�ű��̳߳��˳���..."));
			ui.tableWidget->item(row, column + 1)->setTextColor(Qt::yellow);
			m_roles[row].oldState = m_roles[row].state;
			m_roles[row].state = -2;
			m_roles[row].currentCmdcode = 0;
			print_info(QString::fromLocal8Bit("�˳��ű��߳�..."), role,"#ff0000");
			emit stopRole(m_roles[row].role);
		}
		break;
	}
	case 7:
	{
		if (m_roles[row].state == 1)
		{
			ui.tableWidget->item(row, 2)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, 3)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, 3)->setTextColor(Qt::gray);
			ui.tableWidget->item(row, 7)->setTextColor(Qt::yellow);
			ui.tableWidget->item(row, 5)->setText(QString::fromLocal8Bit("�����ͷſ���Ȩ..."));
			m_roles[row].oldState = 1;
			m_roles[row].state = -2;
			m_roles[row].currentCmdcode = 4;
			print_info(QString::fromLocal8Bit("�ͷŽű�����Ȩ..."), role,"#ff0000");
			emit releaseControl(m_roles[row].role);

		}
		break;
	}
	default:
		break;
	}
}
