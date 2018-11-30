#include "IconFont.h"
#include <QFontDatabase>


IconFont * IconFont::Instance()
{
	static IconFont ins;
	return &ins;
}

bool IconFont::Init()
{
	int id = QFontDatabase::addApplicationFont("fontawesome-webfont.ttf");
	QStringList families = QFontDatabase::applicationFontFamilies(id);
	m_font.setFamily(families.at(0));
	m_font.setPointSize(20);
	return false;
}

QFont & IconFont::GetFont()
{
	// TODO: �ڴ˴����� return ���
	return m_font;
}

IconFont::IconFont()
{
}


IconFont::~IconFont()
{
}
