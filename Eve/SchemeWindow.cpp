#include "SchemeWindow.h"
#include "RoleConfigure.h"
#include "Scheme_1.h"
#include <QHeaderView>
#include <QPoint>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QSettings>

SchemeWindow::SchemeWindow(QString role,QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.pbApply->setEnabled(false);
	m_role = role;
	bar = new QProgressBar(this);
	bar->setMaximum(0);
	bar->setMinimum(0);
	bar->setWindowFlag(Qt::Dialog);
	bar->setWindowModality(Qt::WindowModal);
	shields = new QButtonGroup(this);
	shields->addButton(ui.rbShield,0);
	shields->addButton(ui.rbArmour,1);
	RoleConfigure congfigure;
	if (congfigure.Loadsettings(role))
	{
		QPoint pt;
		QTableWidgetItem *item=nullptr;
		pt = congfigure.GetLocalPos();
		item = new QTableWidgetItem(QString::number(pt.x()));
		item->setTextColor(Qt::green);
		item->setTextAlignment(Qt::AlignVCenter);
		ui.tableWidget->setItem(0, 1, item);
		item = new QTableWidgetItem(QString::number(pt.y()));
		item->setTextColor(Qt::green);
		item->setTextAlignment(Qt::AlignVCenter);
		ui.tableWidget->setItem(0, 2, item);

		pt = congfigure.GetSelectItemPos();
		item = new QTableWidgetItem(QString::number(pt.x()));
		item->setTextColor(Qt::green);
		item->setTextAlignment(Qt::AlignVCenter);
		ui.tableWidget->setItem(1, 1, item);
		item = new QTableWidgetItem(QString::number(pt.y()));
		item->setTextColor(Qt::green);
		item->setTextAlignment(Qt::AlignVCenter);
		ui.tableWidget->setItem(1, 2, item);

		pt = congfigure.GetOverviewPos();
		item = new QTableWidgetItem(QString::number(pt.x()));
		item->setTextColor(Qt::green);
		item->setTextAlignment(Qt::AlignVCenter);
		ui.tableWidget->setItem(2, 1, item);
		item = new QTableWidgetItem(QString::number(pt.y()));
		item->setTextColor(Qt::green);
		item->setTextAlignment(Qt::AlignVCenter);
		ui.tableWidget->setItem(2, 2, item);

		pt = congfigure.GetDronesPos();
		item = new QTableWidgetItem(QString::number(pt.x()));
		item->setTextColor(Qt::green);
		item->setTextAlignment(Qt::AlignVCenter);
		ui.tableWidget->setItem(3, 1, item);
		item = new QTableWidgetItem(QString::number(pt.y()));
		item->setTextColor(Qt::green);
		item->setTextAlignment(Qt::AlignVCenter);
		ui.tableWidget->setItem(3, 2, item);

		pt = congfigure.GetInstrumentPanelPos();
		item = new QTableWidgetItem(QString::number(pt.x()));
		item->setTextColor(Qt::green);
		item->setTextAlignment(Qt::AlignVCenter);
		ui.tableWidget->setItem(4, 1, item);
		item = new QTableWidgetItem(QString::number(pt.y()));
		item->setTextColor(Qt::green);
		item->setTextAlignment(Qt::AlignVCenter);
		ui.tableWidget->setItem(4, 2, item);

		pt = congfigure.GetUndockPos();
		item = new QTableWidgetItem(QString::number(pt.x()));
		item->setTextColor(Qt::green);
		item->setTextAlignment(Qt::AlignVCenter);
		ui.tableWidget->setItem(5, 1, item);
		item = new QTableWidgetItem(QString::number(pt.y()));
		item->setTextColor(Qt::green);
		item->setTextAlignment(Qt::AlignVCenter);
		ui.tableWidget->setItem(5, 2, item);

		m_wrapo = congfigure.GetWrapWithin();
		m_wrapn = m_wrapo;
		ui.cbWrap->setCurrentText(m_wrapo);

		m_shiledarmouro = congfigure.GetShiledArmour();
		m_shiledarmourn = m_shiledarmouro;
		switch (m_shiledarmouro)
		{
		case 0:
			ui.rbShield->setChecked(true);
			break;
		case 1:
			ui.rbArmour->setChecked(true);
			break;
		default:
			break;
		}
	}
	connect(ui.pbwdetected, SIGNAL(clicked()), this, SLOT(detected()));
	connect(shields, SIGNAL(buttonClicked(int)), this, SLOT(shiled_clicked(int)));
	connect(ui.cbWrap, SIGNAL(currentTextChanged(const QString &)), this, SLOT(wrap_changed(const QString &)));
	connect(ui.pbApply, SIGNAL(clicked()), this, SLOT(apply_clicked()));
}

SchemeWindow::~SchemeWindow()
{
	if (runtime != nullptr)
		delete runtime;
}

void SchemeWindow::dectected_finished(bool ok)
{
	bar->hide();
	if (ok)
	{
		RoleConfigure congfigure;
		if (congfigure.Loadsettings(m_role))
		{
			QPoint pt;
			QTableWidgetItem *item = nullptr;
			pt = congfigure.GetLocalPos();
			item = new QTableWidgetItem(QString::number(pt.x()));
			if (ui.tableWidget->item(0, 1)->text() != item->text())
				item->setTextColor(Qt::red);
			else
				item->setTextColor(Qt::green);
			item->setTextAlignment(Qt::AlignVCenter);
			ui.tableWidget->setItem(0, 1, item);
			item = new QTableWidgetItem(QString::number(pt.y()));
			if (ui.tableWidget->item(0, 2)->text() != item->text())
				item->setTextColor(Qt::red);
			else
				item->setTextColor(Qt::green);
			item->setTextAlignment(Qt::AlignVCenter);
			ui.tableWidget->setItem(0, 2, item);

			pt = congfigure.GetSelectItemPos();
			item = new QTableWidgetItem(QString::number(pt.x()));
			if (ui.tableWidget->item(1, 1)->text() != item->text())
				item->setTextColor(Qt::red);
			else
				item->setTextColor(Qt::green);
			item->setTextAlignment(Qt::AlignVCenter);
			ui.tableWidget->setItem(1, 1, item);
			item = new QTableWidgetItem(QString::number(pt.y()));
			if (ui.tableWidget->item(1, 2)->text() != item->text())
				item->setTextColor(Qt::red);
			else
				item->setTextColor(Qt::green);
			item->setTextAlignment(Qt::AlignVCenter);
			ui.tableWidget->setItem(1, 2, item);

			pt = congfigure.GetOverviewPos();
			item = new QTableWidgetItem(QString::number(pt.x()));
			if (ui.tableWidget->item(2, 1)->text() != item->text())
				item->setTextColor(Qt::red);
			else
				item->setTextColor(Qt::green);
			item->setTextAlignment(Qt::AlignVCenter);
			ui.tableWidget->setItem(2, 1, item);
			item = new QTableWidgetItem(QString::number(pt.y()));
			if (ui.tableWidget->item(2, 2)->text() != item->text())
				item->setTextColor(Qt::red);
			else
				item->setTextColor(Qt::green);
			item->setTextAlignment(Qt::AlignVCenter);
			ui.tableWidget->setItem(2, 2, item);

			pt = congfigure.GetDronesPos();
			item = new QTableWidgetItem(QString::number(pt.x()));
			if (ui.tableWidget->item(3, 1)->text() != item->text())
				item->setTextColor(Qt::red);
			else
				item->setTextColor(Qt::green);
			item->setTextAlignment(Qt::AlignVCenter);
			ui.tableWidget->setItem(3, 1, item);
			item = new QTableWidgetItem(QString::number(pt.y()));
			if (ui.tableWidget->item(3, 2)->text() != item->text())
				item->setTextColor(Qt::red);
			else
				item->setTextColor(Qt::green);
			item->setTextAlignment(Qt::AlignVCenter);
			ui.tableWidget->setItem(3, 2, item);

			pt = congfigure.GetInstrumentPanelPos();
			item = new QTableWidgetItem(QString::number(pt.x()));
			if (ui.tableWidget->item(4, 1)->text() != item->text())
				item->setTextColor(Qt::red);
			else
				item->setTextColor(Qt::green);
			item->setTextAlignment(Qt::AlignVCenter);
			ui.tableWidget->setItem(4, 1, item);
			item = new QTableWidgetItem(QString::number(pt.y()));
			if (ui.tableWidget->item(4, 2)->text() != item->text())
				item->setTextColor(Qt::red);
			else
				item->setTextColor(Qt::green);
			item->setTextAlignment(Qt::AlignVCenter);
			ui.tableWidget->setItem(4, 2, item);

			pt = congfigure.GetUndockPos();
			item = new QTableWidgetItem(QString::number(pt.x()));
			if (ui.tableWidget->item(5, 1)->text() != item->text())
				item->setTextColor(Qt::red);
			else
				item->setTextColor(Qt::green);
			item->setTextAlignment(Qt::AlignVCenter);
			ui.tableWidget->setItem(5, 1, item);
			item = new QTableWidgetItem(QString::number(pt.y()));
			if (ui.tableWidget->item(5, 2)->text() != item->text())
				item->setTextColor(Qt::red);
			else
				item->setTextColor(Qt::green);
			item->setTextAlignment(Qt::AlignVCenter);
			ui.tableWidget->setItem(5, 2, item);
		}
		else
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("´íÎó"), QString::fromLocal8Bit("»ñÈ¡¼ì²â½á¹ûÊ§°Ü"));
		}
	}
	else
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("´íÎó"),QString::fromLocal8Bit("´°¿Ú¼ì²âÊ§°Ü"));
	}
}

void SchemeWindow::RoleDetectedInfoSlot(const QString & info, const QString & color)
{
	emit RoleDetectedInfo(m_role, info,color);
}

void SchemeWindow::shiled_clicked(int id)
{
	switch (id)
	{
	case 0:	//¶Ü¿¹
		m_shiledarmourn = 0;
		check_setting_change();
		break;
	case 1: //¼×¿¹
		m_shiledarmourn = 1;
		check_setting_change();
		break;
	default:break;
	}
}

void SchemeWindow::apply_clicked()
{
	QString setting = QString("./setting/") + m_role + "-Setting.ini";
	QSettings settings(setting, QSettings::IniFormat);
	settings.beginGroup("Setting");
	if (m_shiledarmouro != m_shiledarmourn)
	{
		m_shiledarmouro = m_shiledarmourn;
		settings.setValue("ShiledArmour", m_shiledarmourn);
	}
	if (m_wrapo != m_wrapn)
	{
		m_wrapo = m_wrapn;
		settings.setValue("Wrap", m_wrapo);
	}
	settings.endGroup();
	ui.pbApply->setEnabled(false);
}

void SchemeWindow::wrap_changed(const QString &s)
{
	m_wrapn = s;
	check_setting_change();
}

void SchemeWindow::check_setting_change()
{
	if (m_shiledarmouro != m_shiledarmouro)
	{
		ui.pbApply->setEnabled(true);
		return;
	}
	if (m_wrapo != m_wrapn)
	{
		ui.pbApply->setEnabled(true);
		return;
	}
	ui.pbApply->setEnabled(false);
}

void SchemeWindow::detected()
{
	if (runtime == nullptr)
	{
		runtime = new RoleRuntime();
		runtime->role = m_role;
		runtime->thread = new QThread(this);
		runtime->scheme = new Scheme_1(m_role);
		runtime->scheme->moveToThread(runtime->thread);
		runtime->thread->start();
		connect(runtime->scheme, SIGNAL(WindowPositionDetectionFinish(bool)), this, SLOT(dectected_finished(bool)));
		connect(runtime->scheme, SIGNAL(newInfo(const QString&, const QString&)), this, SLOT(RoleDetectedInfoSlot(const QString&, const QString&)));
	}
	runtime->scheme->WindowPositionDetection();
	bar->show();
}
