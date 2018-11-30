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
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OutputInfo
{
public:
    QGridLayout *gridLayout;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
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
        dockWidget = new QDockWidget(OutputInfo);
        dockWidget->setObjectName(QStringLiteral("dockWidget"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        textEdit = new QTextEdit(dockWidgetContents);
        textEdit->setObjectName(QStringLiteral("textEdit"));

        verticalLayout->addWidget(textEdit);

        dockWidget->setWidget(dockWidgetContents);

        gridLayout->addWidget(dockWidget, 0, 0, 1, 1);


        retranslateUi(OutputInfo);

        QMetaObject::connectSlotsByName(OutputInfo);
    } // setupUi

    void retranslateUi(QWidget *OutputInfo)
    {
        OutputInfo->setWindowTitle(QApplication::translate("OutputInfo", "OutputInfo", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OutputInfo: public Ui_OutputInfo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OUTPUTINFO_H
