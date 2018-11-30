#include "Eve.h"
#include "IconFont.h"
#include <QtWidgets/QApplication>
#include <QFontDatabase>
#include <QFile>

#define UNTT 0
#define QUNTT 0


class CommonHelper
{
public:
	static void setStyle(const QString &style)
	{
		QFile qss(style);
		qss.open(QFile::ReadOnly);
		qApp->setStyleSheet(qss.readAll());
		qss.close();
	}
};

#if UNTT
#include "Unitt.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CommonHelper::setStyle("./black.qss");
	IconFont::Instance()->Init();
	Eve w;
	w.show();
	Unitt tt;
	tt.UttProc(argc,argv,8);
	return a.exec();
}
#elif QUNTT
#include "QUnitt.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QUnitt w;
	w.show();
	return a.exec();
}
#else
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CommonHelper::setStyle("./black.qss");
	IconFont::Instance()->Init();
	Eve w;
	w.show();
	return a.exec();
}
#endif // QUNITT



