/*
* main.cpp
*
* @author fl@lekutech
* @date 2020-07-21
* @copuleft GPL 2.0
*/

#include "ClientAutoUpdate.h"
#include <QtWidgets/QApplication>

extern CAU::ClientAutoUpdate* p;



//log日志
void initGlogConfiguration()
{
	QString appRunPath = QCoreApplication::applicationDirPath();
	QString appName = QCoreApplication::applicationName();
	QString logPath = appRunPath + "/logs";
	if (!QDir(logPath).exists())
		QDir().mkpath(logPath);
	google::InitGoogleLogging(appName.toStdString().c_str());
	//设置日志打出至终端
	google::SetStderrLogging(google::GLOG_INFO);
	FLAGS_max_log_size = 20;// MB
	FLAGS_logbufsecs = 0;// flush right now.
	FLAGS_colorlogtostderr = true;


#ifdef LOG_TO_CONSOLE
	google::LogToStderr();
#else
	QString logDestination = logPath + "/" + appName + "_";
	google::SetLogDestination(google::GLOG_INFO, logDestination.toStdString().c_str());
#endif // LOG_TO_CONSOLE
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
	initGlogConfiguration();
	LOG(INFO) << "App start";
    CAU::ClientAutoUpdate* w = new CAU::ClientAutoUpdate;
    w->show();
    return a.exec();
}
