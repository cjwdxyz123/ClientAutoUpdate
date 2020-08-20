/********************************************************************************
** Form generated from reading UI file 'ClientAutoUpdate.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENTAUTOUPDATE_H
#define UI_CLIENTAUTOUPDATE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClientAutoUpdate
{
public:
    QWidget *centralWidget;
    QPushButton *yesButton;
    QPushButton *endButton;
    QLabel *label;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ClientAutoUpdate)
    {
        if (ClientAutoUpdate->objectName().isEmpty())
            ClientAutoUpdate->setObjectName(QStringLiteral("ClientAutoUpdate"));
        ClientAutoUpdate->resize(546, 274);
        centralWidget = new QWidget(ClientAutoUpdate);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        yesButton = new QPushButton(centralWidget);
        yesButton->setObjectName(QStringLiteral("yesButton"));
        yesButton->setGeometry(QRect(110, 170, 101, 41));
        endButton = new QPushButton(centralWidget);
        endButton->setObjectName(QStringLiteral("endButton"));
        endButton->setGeometry(QRect(340, 170, 101, 41));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(70, 40, 451, 91));
        label->setTextFormat(Qt::AutoText);
        ClientAutoUpdate->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(ClientAutoUpdate);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        ClientAutoUpdate->setStatusBar(statusBar);

        retranslateUi(ClientAutoUpdate);

        QMetaObject::connectSlotsByName(ClientAutoUpdate);
    } // setupUi

    void retranslateUi(QMainWindow *ClientAutoUpdate)
    {
        ClientAutoUpdate->setWindowTitle(QApplication::translate("ClientAutoUpdate", "\346\233\264\346\226\260", Q_NULLPTR));
        yesButton->setText(QApplication::translate("ClientAutoUpdate", "\346\230\257", Q_NULLPTR));
        endButton->setText(QApplication::translate("ClientAutoUpdate", "\345\220\246", Q_NULLPTR));
        label->setText(QApplication::translate("ClientAutoUpdate", "\350\257\245\347\250\213\345\272\217\351\234\200\350\246\201\346\233\264\346\226\260\357\274\214\346\230\257\345\220\246\345\215\207\347\272\247\357\274\237", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ClientAutoUpdate: public Ui_ClientAutoUpdate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENTAUTOUPDATE_H
