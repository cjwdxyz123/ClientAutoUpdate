/*
* ProgressDialog class
*
* @author fl@lekutech
* @date 2020-07-21
* @copuleft GPL 2.0
*/

#include "ProgressDialog.h"

#include <QProgressDialog>
#include <QFont>

ProgressDialog::ProgressDialog(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	
}

ProgressDialog::~ProgressDialog()
{
}

void ProgressDialog::run()
{
	QProgressBar *progress = new QProgressBar();
	QLabel *textForProgress = new QLabel;
	progress->setWindowTitle("Update ...");
	/*textForProgress->setText(tr("¸üÐÂÖÐ¡­¡­"));
	textForProgress->setScaledContents(true);*/

    progress->setRange(0, 0);
    progress->setOrientation(Qt::Horizontal);
    progress->setInvertedAppearance(false);
    progress->setVisible(true);
	progress->setFixedSize(500, 35);
}

void ProgressDialog::stop()
{
	
}
