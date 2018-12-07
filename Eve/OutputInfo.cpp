#include "OutputInfo.h"

OutputInfo::OutputInfo(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

OutputInfo::~OutputInfo()
{

}

void OutputInfo::StandOut(const QString & info,const QString& color)
{
	print_info(info,color);
}

void OutputInfo::print_info(const QString & info,const QString &color)
{
	QString msg;
	QString color_info = msg.append("<span style = \"color:");
	color_info.append(color);
	color_info.append(";\">");
	color_info.append(">> ");
	color_info.append(info);
	color_info.append("</span>");
	ui.textEdit->append(color_info);
	ui.textEdit->moveCursor(QTextCursor::End);
}
