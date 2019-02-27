/********************************************************************************
** Form generated from reading UI file 'PCRoot.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PCROOT_H
#define UI_PCROOT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PCRoot
{
public:
    QVBoxLayout *verticalLayout_2;
    QLabel *pcName;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pbEve;
    QPushButton *pbRootReflash;
    QPushButton *pbPathset;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout;
    QListWidget *listRoles;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_2;
    QPushButton *pbAddition_a;
    QPushButton *pbAddition;
    QPushButton *pbRemove;
    QPushButton *pbRemove_a;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *PCRoot)
    {
        if (PCRoot->objectName().isEmpty())
            PCRoot->setObjectName(QStringLiteral("PCRoot"));
        PCRoot->resize(291, 643);
        verticalLayout_2 = new QVBoxLayout(PCRoot);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        pcName = new QLabel(PCRoot);
        pcName->setObjectName(QStringLiteral("pcName"));
        QFont font;
        font.setPointSize(20);
        pcName->setFont(font);
        pcName->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout_2->addWidget(pcName);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(10);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        pbEve = new QPushButton(PCRoot);
        pbEve->setObjectName(QStringLiteral("pbEve"));
        pbEve->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pbEve->sizePolicy().hasHeightForWidth());
        pbEve->setSizePolicy(sizePolicy);
        pbEve->setMaximumSize(QSize(32, 32));
        QIcon icon;
        icon.addFile(QStringLiteral("pic/eve.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QStringLiteral("pic/eve.jpg"), QSize(), QIcon::Normal, QIcon::On);
        icon.addFile(QStringLiteral("pic/eve_on.jpg"), QSize(), QIcon::Disabled, QIcon::Off);
        icon.addFile(QStringLiteral("pic/eve_on.jpg"), QSize(), QIcon::Disabled, QIcon::On);
        icon.addFile(QStringLiteral("pic/eve.jpg"), QSize(), QIcon::Active, QIcon::Off);
        pbEve->setIcon(icon);
        pbEve->setIconSize(QSize(32, 32));
        pbEve->setFlat(true);

        horizontalLayout_2->addWidget(pbEve);

        pbRootReflash = new QPushButton(PCRoot);
        pbRootReflash->setObjectName(QStringLiteral("pbRootReflash"));
        sizePolicy.setHeightForWidth(pbRootReflash->sizePolicy().hasHeightForWidth());
        pbRootReflash->setSizePolicy(sizePolicy);
        pbRootReflash->setMaximumSize(QSize(32, 32));
        pbRootReflash->setFlat(true);

        horizontalLayout_2->addWidget(pbRootReflash);

        pbPathset = new QPushButton(PCRoot);
        pbPathset->setObjectName(QStringLiteral("pbPathset"));
        sizePolicy.setHeightForWidth(pbPathset->sizePolicy().hasHeightForWidth());
        pbPathset->setSizePolicy(sizePolicy);
        pbPathset->setMaximumSize(QSize(48, 32));
        QFont font1;
        font1.setPointSize(6);
        font1.setBold(false);
        font1.setUnderline(false);
        font1.setWeight(50);
        pbPathset->setFont(font1);
        pbPathset->setToolTipDuration(-1);
        pbPathset->setFlat(true);

        horizontalLayout_2->addWidget(pbPathset);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        listRoles = new QListWidget(PCRoot);
        listRoles->setObjectName(QStringLiteral("listRoles"));

        horizontalLayout->addWidget(listRoles);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        pbAddition_a = new QPushButton(PCRoot);
        pbAddition_a->setObjectName(QStringLiteral("pbAddition_a"));
        pbAddition_a->setFlat(true);

        verticalLayout->addWidget(pbAddition_a);

        pbAddition = new QPushButton(PCRoot);
        pbAddition->setObjectName(QStringLiteral("pbAddition"));
        pbAddition->setFlat(true);

        verticalLayout->addWidget(pbAddition);

        pbRemove = new QPushButton(PCRoot);
        pbRemove->setObjectName(QStringLiteral("pbRemove"));
        pbRemove->setFlat(true);

        verticalLayout->addWidget(pbRemove);

        pbRemove_a = new QPushButton(PCRoot);
        pbRemove_a->setObjectName(QStringLiteral("pbRemove_a"));
        pbRemove_a->setFlat(true);

        verticalLayout->addWidget(pbRemove_a);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout);


        verticalLayout_2->addLayout(horizontalLayout);


        retranslateUi(PCRoot);

        QMetaObject::connectSlotsByName(PCRoot);
    } // setupUi

    void retranslateUi(QWidget *PCRoot)
    {
        PCRoot->setWindowTitle(QApplication::translate("PCRoot", "PCRoot", Q_NULLPTR));
        pcName->setText(QApplication::translate("PCRoot", "TextLabel", Q_NULLPTR));
        pbEve->setText(QString());
#ifndef QT_NO_TOOLTIP
        pbRootReflash->setToolTip(QApplication::translate("PCRoot", "\345\210\267\346\226\260", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        pbRootReflash->setText(QString());
#ifndef QT_NO_TOOLTIP
        pbPathset->setToolTip(QApplication::translate("PCRoot", "\350\256\276\347\275\256EVE\345\220\257\345\212\250\350\267\257\345\276\204", "\350\256\276\347\275\256EVE\345\220\257\345\212\250\350\267\257\345\276\204"));
#endif // QT_NO_TOOLTIP
        pbPathset->setText(QApplication::translate("PCRoot", "\302\267\302\267\302\267", Q_NULLPTR));
        pbAddition_a->setText(QString());
        pbAddition->setText(QString());
        pbRemove->setText(QString());
        pbRemove_a->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class PCRoot: public Ui_PCRoot {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PCROOT_H
