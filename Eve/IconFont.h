#pragma once
#include <QFont>
class IconFont
{
public:
	static IconFont* Instance();
	bool Init();
	QFont &GetFont();
private:
	IconFont();
	~IconFont();
	QFont m_font;
};

