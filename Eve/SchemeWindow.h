#pragma once

#include <QWidget>
#include <QThread>
#include <QProgressBar>
#include <QButtonGroup>
#include "ui_SchemeWindow.h"
#include "IScheme.h"

class SchemeWindow : public QWidget
{
	Q_OBJECT

public:
	SchemeWindow(QString role,QWidget *parent = Q_NULLPTR);
	~SchemeWindow();
signals:
	void RoleDetectedInfo(QString role, QString info,QString color);
private slots:
	void detected();
	void dectected_finished(bool);
	void RoleDetectedInfoSlot(const QString &info, const QString &color);
	void shiled_clicked(int id);
	void apply_clicked();
	void wrap_changed(const QString&);
private:
	struct RoleRuntime
	{
		RoleRuntime()
		{

		}
		~RoleRuntime()
		{
			if (thread != nullptr)
			{
				thread->quit();
				thread->wait();
				delete thread;
				thread = nullptr;
			}
			if (scheme != nullptr)
			{
				delete scheme;
				scheme = nullptr;
			}
		}
		QThread *thread = nullptr;
		IScheme *scheme = nullptr;
		QString role;
	};
	void check_setting_change();
private:
	Ui::SchemeWindow ui;
	RoleRuntime *runtime=nullptr;
	QProgressBar *bar=nullptr;
	QButtonGroup *shields=nullptr;
	QString m_role;
	int m_shiledarmouro;
	int m_shiledarmourn;
	QString m_wrapo;
	QString m_wrapn;
};
