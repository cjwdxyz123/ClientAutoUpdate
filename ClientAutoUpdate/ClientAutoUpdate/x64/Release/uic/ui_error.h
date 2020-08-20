/********************************************************************************
** Form generated from reading UI file 'error.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ERROR_H
#define UI_ERROR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_error
{
public:
    QLabel *label;

    void setupUi(QWidget *error)
    {
        if (error->objectName().isEmpty())
            error->setObjectName(QStringLiteral("error"));
        error->resize(443, 240);
        label = new QLabel(error);
        label->setObjectName(QStringLiteral("label"));
        label->setEnabled(true);
        label->setGeometry(QRect(70, 60, 291, 101));

        retranslateUi(error);

        QMetaObject::connectSlotsByName(error);
    } // setupUi

    void retranslateUi(QWidget *error)
    {
        error->setWindowTitle(QApplication::translate("error", "Form", Q_NULLPTR));
        label->setText(QApplication::translate("error", "update failed!", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class error: public Ui_error {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ERROR_H
