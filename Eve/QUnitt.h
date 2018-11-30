#pragma once

#include <QWidget>
#include <QThread>
#include "ui_QUnitt.h"
#include "IScheme.h"

class QUnitt : public QWidget
{
	Q_OBJECT

public:
	QUnitt(QWidget *parent = Q_NULLPTR);
	~QUnitt();
private slots:
	void safe_pause();
	void print_info(IScheme*);
	void quit();
private:
	Ui::QUnitt ui;
	IScheme *m_scheme;
	QThread m_thread;
};
