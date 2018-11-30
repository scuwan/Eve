#include "OutputInfo.h"

OutputInfo::OutputInfo(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

OutputInfo::~OutputInfo()
{
}

void OutputInfo::Monitor(IScheme *scheme)
{
	QVector<QString> info;
	scheme->GetInfo(info);
	for (int i = 0; i < info.size(); ++i)
	{
		print_info(info[i]);
	}
}

void OutputInfo::print_info(QString & info)
{
	QString msg = "> ";
	ui.textEdit->append(msg.append(info));
}
