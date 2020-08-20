/*
* ProgressDialog class
*
* @author fl@lekutech
* @date 2020-07-21
* @copuleft GPL 2.0
*/

#pragma once

#include <QWidget>
#include "ui_ProgressDialog.h"

 
#include <QProgressBar>  
#include <QLabel>
#include <QGridLayout>


class ProgressDialog : public QWidget
{
	Q_OBJECT

public:
	ProgressDialog(QWidget *parent = Q_NULLPTR);
	~ProgressDialog();

	void run();
	void stop();


	 
private:
	Ui::ProgressDialog ui;

};


