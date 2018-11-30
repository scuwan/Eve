/********************************************************************************
** Form generated from reading UI file 'QUnitt.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QUNITT_H
#define UI_QUNITT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QUnitt
{
public:
    QPushButton *p1;

    void setupUi(QWidget *QUnitt)
    {
        if (QUnitt->objectName().isEmpty())
            QUnitt->setObjectName(QStringLiteral("QUnitt"));
        QUnitt->resize(400, 300);
        p1 = new QPushButton(QUnitt);
        p1->setObjectName(QStringLiteral("p1"));
        p1->setGeometry(QRect(20, 20, 101, 51));

        retranslateUi(QUnitt);

        QMetaObject::connectSlotsByName(QUnitt);
    } // setupUi

    void retranslateUi(QWidget *QUnitt)
    {
        QUnitt->setWindowTitle(QApplication::translate("QUnitt", "QUnitt", Q_NULLPTR));
        p1->setText(QApplication::translate("QUnitt", "\345\256\211\345\205\250\346\232\202\345\201\234", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class QUnitt: public Ui_QUnitt {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QUNITT_H
