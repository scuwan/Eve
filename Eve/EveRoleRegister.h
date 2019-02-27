#pragma once

#include <QWidget>
#include <QStringList>
#include "ui_EveRoleRegister.h"

class EveRoleRegister : public QWidget
{
	Q_OBJECT

public:
	EveRoleRegister(QWidget *parent = Q_NULLPTR);
	~EveRoleRegister();

	void SetRoleList(QStringList &roles);
signals:
	void RegisterOk();
private slots:
	void submit();
private:
	Ui::EveRoleRegister ui;
};
