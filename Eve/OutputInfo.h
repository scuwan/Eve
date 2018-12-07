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
	void StandOut(const QString& info, const QString& color);
private:
	void print_info(const QString &info,const QString &color="#ffffff");
private:
	Ui::OutputInfo ui;
};
