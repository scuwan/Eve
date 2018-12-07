/********************************************************************************
** Form generated from reading UI file 'OutputInfo.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OUTPUTINFO_H
#define UI_OUTPUTINFO_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OutputInfo
{
public:
    QGridLayout *gridLayout;
    QTextEdit *textEdit;

    void setupUi(QWidget *OutputInfo)
    {
        if (OutputInfo->objectName().isEmpty())
            OutputInfo->setObjectName(QStringLiteral("OutputInfo"));
        OutputInfo->resize(794, 477);
        gridLayout = new QGridLayout(OutputInfo);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        textEdit = new QTextEdit(OutputInfo);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setReadOnly(true);

        gridLayout->addWidget(textEdit, 0, 0, 1, 1);


        retranslateUi(OutputInfo);

        QMetaObject::connectSlotsByName(OutputInfo);
    } // setupUi

    void retranslateUi(QWidget *OutputInfo)
    {
        OutputInfo->setWindowTitle(QApplication::translate("OutputInfo", "OutputInfo", Q_NULLPTR));
        textEdit->setHtml(QApplication::translate("OutputInfo", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OutputInfo: public Ui_OutputInfo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OUTPUTINFO_H
