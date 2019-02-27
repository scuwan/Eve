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
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionwindowtrans;
    QAction *actionWindow;
    QAction *actionregister;
    QAction *actionrenew;
    QWidget *centralwidget;
    QMenuBar *menubar;
    QMenu *menu;
    QMenu *menu_2;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1419, 857);
        QIcon icon;
        icon.addFile(QStringLiteral("pic/wicon.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setWindowOpacity(0.8);
        MainWindow->setIconSize(QSize(48, 48));
        actionwindowtrans = new QAction(MainWindow);
        actionwindowtrans->setObjectName(QStringLiteral("actionwindowtrans"));
        QIcon icon1;
        icon1.addFile(QStringLiteral("pic/opacity.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionwindowtrans->setIcon(icon1);
        actionWindow = new QAction(MainWindow);
        actionWindow->setObjectName(QStringLiteral("actionWindow"));
        actionregister = new QAction(MainWindow);
        actionregister->setObjectName(QStringLiteral("actionregister"));
        QIcon icon2;
        icon2.addFile(QStringLiteral("pic/register.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionregister->setIcon(icon2);
        actionrenew = new QAction(MainWindow);
        actionrenew->setObjectName(QStringLiteral("actionrenew"));
        QIcon icon3;
        icon3.addFile(QStringLiteral("pic/money.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        actionrenew->setIcon(icon3);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1419, 23));
        menu = new QMenu(menubar);
        menu->setObjectName(QStringLiteral("menu"));
        menu_2 = new QMenu(menubar);
        menu_2->setObjectName(QStringLiteral("menu_2"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menu_2->menuAction());
        menubar->addAction(menu->menuAction());
        menu->addAction(actionwindowtrans);
        menu_2->addAction(actionregister);
        menu_2->addAction(actionrenew);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "EVE \346\254\247\346\234\215", Q_NULLPTR));
        actionwindowtrans->setText(QApplication::translate("MainWindow", "\347\252\227\345\217\243\351\200\217\346\230\216\345\272\246", Q_NULLPTR));
        actionwindowtrans->setIconText(QApplication::translate("MainWindow", "\347\252\227\345\217\243\351\200\217\346\230\216\345\272\246", Q_NULLPTR));
        actionWindow->setText(QApplication::translate("MainWindow", "\347\252\227\345\217\243", Q_NULLPTR));
        actionregister->setText(QApplication::translate("MainWindow", "\346\263\250\345\206\214", Q_NULLPTR));
        actionrenew->setText(QApplication::translate("MainWindow", "\347\273\255\350\264\271", Q_NULLPTR));
        menu->setTitle(QApplication::translate("MainWindow", "\350\256\276\347\275\256", Q_NULLPTR));
        menu_2->setTitle(QApplication::translate("MainWindow", "\346\263\250\345\206\214", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EVE_H
