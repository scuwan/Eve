#pragma once

#include <QWidget>
#include <QStringList>
#include <QFileDialog>
#include <QProgressBar>
#include <QTimer>
#include "ui_PCRoot.h"
#include "EveRoleRegister.h"

class PCRoot : public QWidget
{
	Q_OBJECT

public:
	PCRoot(QWidget *parent = Q_NULLPTR);
	~PCRoot();
private slots:
	void reflash_clicked();
	void addition_clicked();
	void addition_all_clicked();
	void remove_clicked();
	void remove_all_clicked();
	void set_eve_launcher();
	void launch_eve();
	void time_out();
	void itemDoubleClicked(QListWidgetItem *item);
	void register_ok();
private:
	Ui::PCRoot ui;
	QProgressBar *bar;
	QTimer *timer;
	EveRoleRegister *evereg=nullptr;
	int timeouts = 15;
	QStringList m_authorized;
	QStringList m_lists;
	QString m_eveLuncherPath = "";
	void init();
	void parse_role_list();
	bool is_authorized(QString role);
	bool is_exist(QString role);
};
