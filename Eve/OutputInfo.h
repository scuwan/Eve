#pragma once

#include <QWidget>
#include "ui_OutputInfo.h"
#include "IScheme.h"

class OutputInfo : public QWidget
{
	Q_OBJECT

public:
	OutputInfo(QWidget *parent = Q_NULLPTR);
	~OutputInfo();
public slots:
	void Monitor(IScheme*);
private:
	void print_info(QString &info);
private:
	Ui::OutputInfo ui;
};
