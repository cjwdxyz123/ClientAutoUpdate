/*
* error class
*
* @author fl@lekutech
* @date 2020-07-21
* @copuleft GPL 2.0
*/

#pragma once

#include <QWidget>
#include "ui_error.h"

class error : public QWidget
{
	Q_OBJECT

public:
	error(QWidget *parent = Q_NULLPTR);
	~error();

public:
	Ui::error ui;
};
