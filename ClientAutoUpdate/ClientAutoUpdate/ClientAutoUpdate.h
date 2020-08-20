/*
* Client autoUpdate class
*
* @author fl@lekutech
* @date 2020-07-21
* @copuleft GPL 2.0
*/

#ifndef __CLIENTAUTOUPDATE_H__
#define __CLIENTAUTOUPDATE_H__

#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QDateTime>
#include <QUuid>
#include <QString> 
#include <QUrl> 
#include <QDir> 
#include <QDomElement> 
#include <QDomDocument> 
#include <QDomNode>
#include <QCryptographicHash>
#include "ui_ClientAutoUpdate.h"

#include "libcurl.h"
#include "error.h"
#include "ProgressDialog.h"
#include "Downloader.h"

#include "windows.h"
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "glog\logging.h"

#include "cstdlib"
#include <direct.h>    
#include <io.h>  
#include <tlhelp32.h>
#include <tchar.h>
#include <ShellAPI.h>

#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

namespace CAU
{

	class ClientAutoUpdate : public QMainWindow
	{
		Q_OBJECT

	public:
		ClientAutoUpdate(QWidget* parent = Q_NULLPTR);
	public:
		void readLocalXML();//解析本地XML文件
		void checkVersion();//检测客户端版本的HTTP请求
		void compareMode();//对返回的数据分析以确定更新模式（增量、全量）
		void requestUpdate();//更新的HTTP请求
		//void readDownloadXML();//解析增量更新的XML文件
		void readFileNode(const QDomNode& node, std::unordered_map<std::string, std::string>& fileMd5s);
		void readFiles(const std::string& localFile, std::unordered_map<std::string, std::string>& localFileMd5s, std::unordered_map<std::string, std::string>& localMergeFileMd5s);
		void compareXML();//对比本地和下载的XML
		void download();//通过HTTP请求下载（增量、全量）更新需要的文件
		void process();//进程处理
		bool findProcess(std::string strProcessName, DWORD& nPid);//找到指定进程
		bool KillProcess1(DWORD dwPid);//杀死指定进程
		void backup();//更新前对用户本地文件的备份
		void partialUpdate();//执行增量更新
		void allUpdate();//执行全量更新
		void endUpdate();//更新完毕，启动指定进程
		void recovery();//更新失败的版本回退
		QString signUrl(const QString&urlPath);//uri签名函数
	//	QString invokeRestMethod(const QString& restName, const QString& data);
	private:
		Ui::ClientAutoUpdate ui;
		error* err;
		ProgressDialog* prg;
	private:
		//本地的XML配置文件信息
		std::unordered_map<std::string, std::string> localCommonPath_md5, localMergePath_md5;
		//服务器发送过来的XML配置文件信息
		std::unordered_map<std::string, std::string> requestCommonPath_md5, requestMergePath_md5;
		//替换的部分
		std::unordered_map<std::string, std::string> replaceCommonPath_md5;

		std::string localVersion, requestVersion;
		std::string productName;
		std::string requestURL = "";
		std::string localPath = "../updateconfig.xml";
		std::string httpJsonToString, fileServer, uri, md5;
		std::string uriReturnXml;
		int mode = 0;
		std::string killProcess = "";
		std::string processStartAfterUpdate = "";
		std::string IP;
		std::string fileReturnFile;
		private slots:
		void go();//用户点击确定更新后程序的执行顺序
		void stop();//用户不更新时的弹窗选择
	};
}//namespace CAU

#endif // !__CLIENTAUTOUPDATE_H__