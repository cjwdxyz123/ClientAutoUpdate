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
		void readLocalXML();//��������XML�ļ�
		void checkVersion();//���ͻ��˰汾��HTTP����
		void compareMode();//�Է��ص����ݷ�����ȷ������ģʽ��������ȫ����
		void requestUpdate();//���µ�HTTP����
		//void readDownloadXML();//�����������µ�XML�ļ�
		void readFileNode(const QDomNode& node, std::unordered_map<std::string, std::string>& fileMd5s);
		void readFiles(const std::string& localFile, std::unordered_map<std::string, std::string>& localFileMd5s, std::unordered_map<std::string, std::string>& localMergeFileMd5s);
		void compareXML();//�Աȱ��غ����ص�XML
		void download();//ͨ��HTTP�������أ�������ȫ����������Ҫ���ļ�
		void process();//���̴���
		bool findProcess(std::string strProcessName, DWORD& nPid);//�ҵ�ָ������
		bool KillProcess1(DWORD dwPid);//ɱ��ָ������
		void backup();//����ǰ���û������ļ��ı���
		void partialUpdate();//ִ����������
		void allUpdate();//ִ��ȫ������
		void endUpdate();//������ϣ�����ָ������
		void recovery();//����ʧ�ܵİ汾����
		QString signUrl(const QString&urlPath);//uriǩ������
	//	QString invokeRestMethod(const QString& restName, const QString& data);
	private:
		Ui::ClientAutoUpdate ui;
		error* err;
		ProgressDialog* prg;
	private:
		//���ص�XML�����ļ���Ϣ
		std::unordered_map<std::string, std::string> localCommonPath_md5, localMergePath_md5;
		//���������͹�����XML�����ļ���Ϣ
		std::unordered_map<std::string, std::string> requestCommonPath_md5, requestMergePath_md5;
		//�滻�Ĳ���
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
		void go();//�û����ȷ�����º�����ִ��˳��
		void stop();//�û�������ʱ�ĵ���ѡ��
	};
}//namespace CAU

#endif // !__CLIENTAUTOUPDATE_H__