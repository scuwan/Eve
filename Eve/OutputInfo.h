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
	void StandOut(const QString& info);
private:
	void print_info(const QString &info);
private:
	Ui::OutputInfo ui;
};
