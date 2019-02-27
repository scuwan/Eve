/********************************************************************************
** Form generated from reading UI file 'EveRoleRegister.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EVEROLEREGISTER_H
#define UI_EVEROLEREGISTER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EveRoleRegister
{
public:
    QHBoxLayout *horizontalLayout;
    QListWidget *listWidget;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QPushButton *pbSubmit;

    void setupUi(QWidget *EveRoleRegister)
    {
        if (EveRoleRegister->objectName().isEmpty())
            EveRoleRegister->setObjectName(QStringLiteral("EveRoleRegister"));
        EveRoleRegister->resize(400, 300);
        horizontalLayout = new QHBoxLayout(EveRoleRegister);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        listWidget = new QListWidget(EveRoleRegister);
        listWidget->setObjectName(QStringLiteral("listWidget"));

        horizontalLayout->addWidget(listWidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        pbSubmit = new QPushButton(EveRoleRegister);
        pbSubmit->setObjectName(QStringLiteral("pbSubmit"));

        verticalLayout->addWidget(pbSubmit);


        horizontalLayout->addLayout(verticalLayout);


        retranslateUi(EveRoleRegister);

        QMetaObject::connectSlotsByName(EveRoleRegister);
    } // setupUi

    void retranslateUi(QWidget *EveRoleRegister)
    {
        EveRoleRegister->setWindowTitle(QApplication::translate("EveRoleRegister", "EveRoleRegister", Q_NULLPTR));
        pbSubmit->setText(QApplication::translate("EveRoleRegister", "\346\217\220\344\272\244", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class EveRoleRegister: public Ui_EveRoleRegister {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EVEROLEREGISTER_H
