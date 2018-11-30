/********************************************************************************
** Form generated from reading UI file 'Eve.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EVE_H
#define UI_EVE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EveClass
{
public:

    void setupUi(QWidget *EveClass)
    {
        if (EveClass->objectName().isEmpty())
            EveClass->setObjectName(QStringLiteral("EveClass"));
        EveClass->resize(1253, 786);

        retranslateUi(EveClass);

        QMetaObject::connectSlotsByName(EveClass);
    } // setupUi

    void retranslateUi(QWidget *EveClass)
    {
        EveClass->setWindowTitle(QApplication::translate("EveClass", "Eve\346\254\247\346\234\215", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class EveClass: public Ui_EveClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EVE_H
