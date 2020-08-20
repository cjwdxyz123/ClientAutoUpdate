/*
* Client autoUpdate class
*
* @author fl@lekutech
* @date 2020-07-21
* @copuleft GPL 2.0
*/

#include "ClientAutoUpdate.h"

using std::endl;
using std::cin;
using std::cout;
#include <QNetworkRequest>
#include <QNetworkReply>
#include <memory>
#include <QSslConfiguration>

#include <qeventloop.h>

static bool copyRecursively(const QString &srcFilePath, const QString &tgtFilePath)
{
	QFileInfo srcFileInfo(srcFilePath);
	if (srcFileInfo.isDir()) {
		if (srcFileInfo.fileName() == "updater")
			return true;
		QDir targetDir(tgtFilePath);
		targetDir.cdUp();
		if (!QDir().mkpath(tgtFilePath))
			return false;
		QDir sourceDir(srcFilePath);
		QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
		foreach(const QString &fileName, fileNames) {
			const QString newSrcFilePath = srcFilePath + QLatin1Char('/') + fileName;
			const QString newTgtFilePath = tgtFilePath + QLatin1Char('/') + fileName;
			if (!copyRecursively(newSrcFilePath, newTgtFilePath))
				return false;
		}
	}
	else {
		if (!QFile::copy(srcFilePath, tgtFilePath))
			return false;
	}
	return true;
}


QString invokeRestMethod(const QString& restName, const QString& data)
{
	QNetworkRequest request;
	QSslConfiguration sslConfig;

	sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
	sslConfig.setProtocol(QSsl::TlsV1SslV3);
	request.setSslConfiguration(sslConfig);

	request.setUrl(QUrl(restName));
	request.setRawHeader("Content-Type", "application/json");
	request.setRawHeader("CLIENTVERSION", ("V1"));
	QNetworkAccessManager* manager = new QNetworkAccessManager;

	std::unique_ptr<QNetworkReply> reply(manager->post(request, data.toLocal8Bit()));
	QEventLoop loop;
	QObject::connect(reply.get(), SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	QString replyMsg = reply->readAll();
	QNetworkReply::NetworkError error = reply->error();
	if (error != QNetworkReply::NoError)
	{
		LOG(ERROR) << "download file failed.";
		return "";
	}
	return replyMsg;
}
//
CAU::ClientAutoUpdate::ClientAutoUpdate(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	err = new error;
	QFont ft;
	ft.setPointSize(18);
	this->ui.label->setFont(ft);
	connect(this->ui.yesButton, SIGNAL(clicked(bool)), this, SLOT(go()));
	connect(this->ui.endButton, SIGNAL(clicked(bool)), this, SLOT(stop()));
}

namespace CAU
{
	//用户点击确定更新后程序的执行顺序
	void ClientAutoUpdate::go()
	{
		LOG(INFO) << "start ProgressDialog";
		prg->run();
		LOG(INFO) << "end ProgressDialog";

		LOG(INFO) << "start readLocalXML";
		//读取本地xml确定当前版本号
		readLocalXML();
		readFiles(localPath, localCommonPath_md5, localMergePath_md5);
		LOG(INFO) << "end readLocalXML";

		LOG(INFO) << "start checkVersion";
		//将版本号通过http请求发送到服务端
		checkVersion();
		LOG(INFO) << "end checkVersion";

		LOG(INFO) << "start compareMode";
		//对服务端返回的数据分析以确定更新模式（增量、全量）
		compareMode();
		LOG(INFO) << "end compareMode";

		LOG(INFO) << "start requestUpdate";
		//发送uri下载xml或者exe
		requestUpdate();
		LOG(INFO) << "end requestUpdate";

		//若是增量更新
		if (mode == 0)
		{
			//LOG(INFO) << "start readDownloadXML";
			////解析增量更新的XML文件
   //         readDownloadXML();
			//LOG(INFO) << "end readDownloadXML";

			readFiles("./DownloadSoft/updateconfig.xml", requestCommonPath_md5, requestMergePath_md5);


			LOG(INFO) << "start compareXML";
			//对比两个xml以确定各个文件的变动
			compareXML();
			LOG(INFO) << "end compareXML";

			LOG(INFO) << "start download";
			//下载变动的文件
			download();
			LOG(INFO) << "end download";
		}

		LOG(INFO) << "start backup";
		//对本地文件进行备份
		/*backup();*/
		copyRecursively(QFileInfo(QApplication::applicationDirPath()).absolutePath(), QApplication::applicationDirPath() + "/backup/");
		LOG(INFO) << "end backup";

		LOG(INFO) << "start process";
		//查找进程、杀死进程
		process();
		LOG(INFO) << "end process";



		//增量更新
		if (mode == 0)
		{
			LOG(INFO) << "start partialUpdate";
			//执行增量更新
			partialUpdate();
			LOG(INFO) << "end partialUpdate";

			LOG(INFO) << "start endUpdate";
			//更新完成
			endUpdate();
			LOG(INFO) << "end endUpdate";
		}

		//全量更新
		if (mode == 1)
		{
			LOG(INFO) << "start allUpdate";
			//执行全量更新
			allUpdate();
			LOG(INFO) << "end allUpdate";
		}

		LOG(INFO) << "finish!";
	}

	//解析本地XML文件
	void ClientAutoUpdate::readLocalXML()
	{
		try
		{
			std::fstream file, fileName, fileIP;
			std::string bufpath, bufmd5, bufFromFile;
			//flag=1表示common，flag2表示merge
			int length, flag = 0;
			std::string::size_type lengthIP, lengthPath, lengthMd5, lengthVersion, lengthProduct;
			file.open(localPath);
			fileName.open("../version.ini");
			fileIP.open("./config.ini");

			if (!file.is_open())
			{
				throw(1);
			}
			if (!file.is_open())
			{
				throw(1);
			}

			/*while (!file.eof())
			{
				file >> bufFromFile;

				if (bufFromFile.find("<Common>") != std::string::npos)
				{
					flag = 1;
				}

				if (bufFromFile.find("<Merge>") != std::string::npos)
				{
					flag = 2;
				}

				if (flag == 1)
				{
					if (bufFromFile.find("<path>") != std::string::npos)
					{
						length = bufFromFile.length();
						lengthPath = bufFromFile.find("</path>");
						bufpath.assign(bufFromFile, 6, lengthPath - 6);
						localCommonPath_md5[bufpath] = bufmd5;
					}

					if (bufFromFile.find("<MD5>") != std::string::npos)
					{
						length = bufFromFile.length();
						lengthMd5 = bufFromFile.find("</MD5>");
						bufmd5.assign(bufFromFile, 5, lengthMd5 - 5);
						localCommonPath_md5[bufpath] = bufmd5;
					}
				}

				if (flag == 2)
				{
					if (bufFromFile.find("<path>") != std::string::npos)
					{
						length = bufFromFile.length();
						lengthPath = bufFromFile.find("</path>");
						bufpath.assign(bufFromFile, 6, lengthPath - 6);
						localMergePath_md5[bufpath] = bufmd5;
					}

					if (bufFromFile.find("<MD5>") != std::string::npos)
					{
						length = bufFromFile.length();
						lengthMd5 = bufFromFile.find("</MD5>");
						bufmd5.assign(bufFromFile, 5, lengthMd5 - 5);
						localMergePath_md5[bufpath] = bufmd5;
					}
				}
			}*/
			while (!fileName.eof())
			{
				fileName >> bufFromFile;
				int index = 0;
				if (!bufFromFile.empty())//清除空格
				{
					while ((index = bufFromFile.find(' ', index)) != std::string::npos)
					{
						bufFromFile.erase(index, 1);
					}
				}
				for (int i = 0; i < bufFromFile.length(); i++)//大写转小写
				{
					if (bufFromFile[i] >= 'A'&&bufFromFile[i] <= 'Z')
					{
						bufFromFile[i] += 32;
					}
					else
						continue;
				}

				if (bufFromFile.find("version=") != std::string::npos)
				{
					lengthVersion = bufFromFile.find("version=");
					length = bufFromFile.length();
					localVersion.assign(bufFromFile, 8, lengthVersion - 8);
				}
				if (bufFromFile.find("product=") != std::string::npos)
				{
					lengthProduct = bufFromFile.find("product=");
					length = bufFromFile.length();
					productName.assign(bufFromFile, 8, lengthProduct - 8);
				}
			}

			while (!fileIP.eof())
			{
				fileIP >> bufFromFile;
				//int index = 0;
				//if (!bufFromFile.empty())//清除空格
				//{
				//	while ((index = bufFromFile.find(' ', index)) != std::string::npos)
				//	{
				//		bufFromFile.erase(index, 1);
				//	}
				//}
				//for (int i = 0; i < bufFromFile.length(); i++)//大写转小写
				//{
				//	if (bufFromFile[i] >= 'A'&&bufFromFile[i] <= 'Z')
				//	{
				//		bufFromFile[i] += 32;
				//	}
				//	else
				//		continue;
				//}

				if (bufFromFile.find("IP:") != std::string::npos)
				{
					lengthIP = bufFromFile.find("IP:");
					length = bufFromFile.length();
					IP.assign(bufFromFile, 3, lengthIP - 3);
				}

			}

			file.close();
			requestURL = "https://" + IP + "/management/httpservice/requestupdate?product=" + productName + "&version=" + localVersion;
		}
		catch (int a)
		{
			err->show();
			exit(1);
		}

	}


	//检测客户端版本的HTTP请求
	void ClientAutoUpdate::checkVersion()
	{
		httpJsonToString = libcurl::libcurl_get(requestURL);
	}

	//对返回的数据分析以确定更新模式（增量、全量）
	void ClientAutoUpdate::compareMode()
	{
		int position = httpJsonToString.find("mode");

		if (position != httpJsonToString.npos)
		{
			mode = atoi(httpJsonToString.substr(position + 7, 1).c_str());
		}
		else {

			exit(1);
		}

		if (mode == 0)
		{
			int fileServerStartPos = httpJsonToString.find("\"fileServer\":\"");
			int fileServerEndPos = httpJsonToString.find("\"", fileServerStartPos + 14);
			fileServer = httpJsonToString.substr(fileServerStartPos + 14, fileServerEndPos - fileServerStartPos - 14);
		}

		int uriStartPos = httpJsonToString.find("\"uri\":\"");
		int uriEndPos = httpJsonToString.find("\"", uriStartPos + 7);
		uri = httpJsonToString.substr(uriStartPos + 7, uriEndPos - uriStartPos - 7);
		QString sign = signUrl(QString::fromStdString(uri));
		uri = uri + "?" + sign.toStdString();
		int md5StartPos = httpJsonToString.find("\"md5\":\"");
		int md5EndPos = httpJsonToString.find("\"", md5StartPos + 7);
		md5 = httpJsonToString.substr(md5StartPos + 7, md5EndPos - md5StartPos - 7);
	}


	//（增、全）量更新的HTTP请求
	void ClientAutoUpdate::requestUpdate()
	{
		std::string dirName = "./DownloadSoft/";
		if (_access(dirName.c_str(), 0) == -1)
		{
			_mkdir(dirName.c_str());
		}
		if (mode == 0) {
			QString fileContent = invokeRestMethod(QString::fromStdString(uri), "");
			//uriReturnXml = libcurl::libcurl_get(uri);
			std::ofstream file;
			file.open("./DownloadSoft/updateconfig.xml", std::ios::trunc);
			file << fileContent.toStdString();
			file.flush();
			file.close();

			/*file.open(".");
			while(file.)*/

		}
		if (mode == 1) {
			CDownloader murl;
			DLIO mDlWork;
			strcpy(mDlWork.url, (char *)uri.data());
			strcpy(mDlWork.filePath, "./DownloadSoft/");
			murl.AddDownloadWork(mDlWork);      //添加到下载任务中
			murl.StartDownloadThread();         //开启下载线程
		}
	}

	//解析增量更新的XML文件
   // void ClientAutoUpdate::readDownloadXML()
   // {
   //     std::fstream file;
   //     std::string bufpath, bufmd5, bufFromFile;
   //     //flag=1表示common，flag2表示merge，flag3表示version
   //     int length, flag = 0;
   //     std::string::size_type lengthPath, lengthMd5, lengthVersion;



   //     file.open("./DownloadSoft/updateconfig.xml",std::ios::in|std::ios::out);

   //     if(!file.is_open())
   //     {
			//err->show();
   //         exit(1);
   //     }

   //     while(!file.eof())
   //     {
   //         file >> bufFromFile;

   //         if(bufFromFile.find("<Common>") != std::string::npos)
   //         {
   //             flag = 1;
   //         }

   //         if(bufFromFile.find("<Merge>") != std::string::npos)
   //         {
   //             flag = 2;
   //         }

   //         if(bufFromFile.find("version") != std::string::npos)
   //         {
   //             flag = 3;
   //         }

   //         if(flag == 1)
   //         {
   //             if(bufFromFile.find("<path>") != std::string::npos)
   //             {
   //                 length = bufFromFile.length();
   //                 lengthPath = bufFromFile.find("</path>");
   //                 bufpath.assign(bufFromFile, 6, lengthPath - 6);
   //                 requestCommonPath_md5[bufpath] = bufmd5;
   //             }

   //             if(bufFromFile.find("<MD5>") != std::string::npos)
   //             {
   //                 length = bufFromFile.length();
   //                 lengthMd5 = bufFromFile.find("</MD5>");
   //                 bufmd5.assign(bufFromFile, 5, lengthMd5 - 5);
   //                 requestCommonPath_md5[bufpath] = bufmd5;
   //             }
   //         }

   //         if(flag == 2)
   //         {
   //             if(bufFromFile.find("<path>") != std::string::npos)
   //             {
   //                 length = bufFromFile.length();
   //                 lengthPath = bufFromFile.find("</path>");
   //                 bufpath.assign(bufFromFile, 6, lengthPath - 6);
   //                 requestMergePath_md5[bufpath] = bufmd5;
   //             }

   //             if(bufFromFile.find("<MD5>") != std::string::npos)
   //             {
   //                 length = bufFromFile.length();
   //                 lengthMd5 = bufFromFile.find("</MD5>");
   //                 bufmd5.assign(bufFromFile, 5, lengthMd5 - 5);
   //                 requestMergePath_md5[bufpath] = bufmd5;
   //             }
   //         }

   //         if(flag == 3)
   //         {
   //             lengthVersion = bufFromFile.find("version=");
   //             length = bufFromFile.length();
   //             requestVersion.assign(bufFromFile, 9, length - 10);
   //             flag = 0;
   //         }
   //     }

   //     file.close();
   // }

	void ClientAutoUpdate::readFileNode(const QDomNode& node, std::unordered_map<std::string, std::string>& fileMd5s)
	{
		std::pair<std::string, std::string> fileMd5Pair;
		QDomNode childNode = node.firstChild();
		while (!childNode.isNull())
		{
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == "path")
			{
				fileMd5Pair.first = childElement.text().toStdString();
			}
			else if (childElement.tagName() == "MD5")
			{
				fileMd5Pair.second = childElement.text().toStdString();
			}
			childNode = childNode.nextSibling();
		}
		fileMd5s.insert(fileMd5Pair);
	}

	void ClientAutoUpdate::readFiles(const std::string& localFile, std::unordered_map<std::string, std::string>& localFileMd5s, std::unordered_map<std::string, std::string>& localMergeFileMd5s)
	{
		QFile lfile(QString::fromStdString(localFile));
		if (!lfile.open(QFile::ReadOnly | QFile::Text))
		{
			exit(1);
		}
		QString errorMsg;
		QDomDocument doc;
		if (!doc.setContent(&lfile, &errorMsg))
		{
			exit(1);
		}
		QDomElement e = doc.documentElement();
		if (e.tagName() != "UpdateConfig")
		{
			exit(1);
		}

		QDomNode node = e.firstChild();
		while (!node.isNull())
		{
			if (node.isElement())
			{
				QDomElement &e1 = node.toElement();
				if (e1.tagName() == "Common")
				{
					QDomNode childNode = node.firstChild();
					while (!childNode.isNull())
					{
						QDomElement e2 = childNode.toElement();
						if (e2.tagName() == "file")
						{
							readFileNode(e2, localFileMd5s);
						}
						childNode = childNode.nextSibling();
					}
				}
				else if (e1.tagName() == "Merge")
				{
					QDomNode childNode = node.firstChild();
					while (!childNode.isNull())
					{
						QDomElement e2 = childNode.toElement();
						if (e2.tagName() == "file")
						{
							readFileNode(e2, localMergeFileMd5s);
						}
						childNode = childNode.nextSibling();
					}
				}
			}
			node = node.nextSibling();
		}
	}





	//对比本地和下载的XML
	void ClientAutoUpdate::compareXML()
	{
		auto localCommonIt = localCommonPath_md5.begin();
		auto requestCommonIt = requestCommonPath_md5.begin();
		int n = 1, m;
		while (localCommonIt != localCommonPath_md5.end())
		{
			/*LOG(INFO) << n++;
			LOG(INFO) << localCommonIt->first;
			LOG(INFO) << localCommonIt->second;
			m = 1;*/
			n++;
			m = 0;
			while (requestCommonIt != requestCommonPath_md5.end())
			{
				m++;
				if (localCommonIt->first == requestCommonIt->first)
				{
					//名称相同且md5码也相同时，不需要变动
					//localCommon的map里存放的是需要删除的部分
					//requestCommon的map里存放的是需要下载并添加的部分
					if (localCommonIt->second == requestCommonIt->second)
					{
						localCommonPath_md5.erase(localCommonIt++);
						requestCommonPath_md5.erase(requestCommonIt);
						requestCommonIt = requestCommonPath_md5.begin();
						break;
					}
				}
				requestCommonIt++;
			}
			localCommonIt++;
			/*if(localCommonIt != localCommonPath_md5.end())
			localCommonIt++;*/
			requestCommonIt = requestCommonPath_md5.begin();
		}
	}

	//通过HTTP请求下载（增量）更新需要的文件
	void ClientAutoUpdate::download()
	{
		try
		{
			CDownloader murl;
			DLIO mDlWork;

			if (mode == 0)
			{
				std::string downloadCommonFile, downloadMergeFile;
				auto requestCommonFileIt = requestCommonPath_md5.begin();
				auto requestMergeFileIt = requestMergePath_md5.begin();


				std::string dirName = "./DownloadCommonFile/";
				if (_access(dirName.c_str(), 0) == -1)
				{
					_mkdir(dirName.c_str());
				}

				while (requestCommonFileIt != requestCommonPath_md5.end())
				{
					downloadCommonFile = fileServer + requestCommonFileIt->first;
					QString sign1 = signUrl(QString::fromStdString(downloadCommonFile));
					downloadCommonFile = downloadCommonFile + "?" + sign1.toStdString();

					fileReturnFile = libcurl::libcurl_get(downloadCommonFile);
					//QFile file(QString("./DownloadCommonFile/") + QString::fromStdString(requestCommonFileIt->first));
					//if (file.open(QFile::WriteOnly | QFile::Text))
					//{
					//	file.write(fileReturnFile.c_str());
					//	file.close();
					//}
					int pos = 0;
					int length = requestCommonFileIt->first.length();
					std::string subDirName, dirName;
					while ((pos = requestCommonFileIt->first.find("/", pos)) != std::string::npos) {
						subDirName = requestCommonFileIt->first.substr(0, pos + 1);
						dirName = "./DownloadCommonFile/" + subDirName;
						if (_access(dirName.c_str(), 0) == -1)
						{
							_mkdir(dirName.c_str());
						}
						pos++;
					}

					std::ofstream file;
					file.open("./DownloadCommonFile/" + requestCommonFileIt->first);
					file << fileReturnFile;
					file.flush();
					file.close();
					//strcpy(mDlWork.url, downloadCommonFile.data());
					//strcpy(mDlWork.filePath, "./DownloadCommonFile/");
					//murl.AddDownloadWork(mDlWork);      //添加到下载任务中
					//murl.StartDownloadThread();         //开启下载线程
					requestCommonFileIt++;
				}

				dirName = "./DownloadMergeFile/";
				if (_access(dirName.c_str(), 0) == -1)
				{
					_mkdir(dirName.c_str());
				}

				while (requestMergeFileIt != requestMergePath_md5.end())
				{
					downloadMergeFile = fileServer + requestMergeFileIt->first;
					QString sign1 = signUrl(QString::fromStdString(downloadCommonFile));
					downloadCommonFile = downloadCommonFile + "?" + sign1.toStdString();

					fileReturnFile = libcurl::libcurl_get(downloadCommonFile);

					int pos = 0;
					int length = requestMergeFileIt->first.length();
					std::string subDirName, dirName;
					while ((pos = requestMergeFileIt->first.find("/", pos)) != std::string::npos) {
						subDirName = requestMergeFileIt->first.substr(0, pos + 1);
						dirName = "./DownloadMergeFile/" + subDirName;
						if (_access(dirName.c_str(), 0) == -1)
						{
							_mkdir(dirName.c_str());
						}
						pos++;
					}
					std::ofstream file;
					file.open("./DownloadMergeFile/" + requestMergeFileIt->first);
					file << fileReturnFile;
					file.close();


					//strcpy(mDlWork.url, downloadMergeFile.data());
					//strcpy(mDlWork.filePath, "./DownloadMergeFile/");
					//murl.AddDownloadWork(mDlWork);      //添加到下载任务中
					//murl.StartDownloadThread();         //开启下载线程
					requestMergeFileIt++;
				}
			}
		}
		catch (int a)
		{
			err->show();
			exit(1);
		}


	}

	//进程处理
	void ClientAutoUpdate::process()
	{
		std::fstream file;
		file.open("./config.ini", std::ios::in | std::ios::out);
		std::string bufFromFile;
		std::string s = "killProcess:";
		int length = s.length();
		while (!file.eof()) {
			file >> bufFromFile;
			int lengthKillProcess;
			if (bufFromFile.find("killProcess:") != std::string::npos)
			{
				lengthKillProcess = bufFromFile.find("killProcess:");
				killProcess.assign(bufFromFile, length, lengthKillProcess - length);
			}
		}

		DWORD pid;
		findProcess(killProcess, pid);
		KillProcess1(pid);
	}

	//找到指定进程
	bool ClientAutoUpdate::findProcess(std::string strProcessName, DWORD& nPid)
	{


		TCHAR tszProcess[64] = { 0 };
		lstrcpy(tszProcess, _T(strProcessName.c_str()));
		//查找进程
		STARTUPINFO st;
		PROCESS_INFORMATION pi;
		PROCESSENTRY32 ps;
		HANDLE hSnapshot;
		memset(&st, 0, sizeof(STARTUPINFO));
		st.cb = sizeof(STARTUPINFO);
		memset(&ps, 0, sizeof(PROCESSENTRY32));
		ps.dwSize = sizeof(PROCESSENTRY32);
		memset(&pi, 0, sizeof(PROCESS_INFORMATION));
		// 遍历进程 
		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnapshot == INVALID_HANDLE_VALUE)
			return false;
		if (!Process32First(hSnapshot, &ps))
			return false;
		do {
			if (lstrcmp(ps.szExeFile, tszProcess) == 0)
			{
				//找到指定的程序
				nPid = ps.th32ProcessID;
				CloseHandle(hSnapshot);
				return true;
				//return dwPid;
			}
		} while (Process32Next(hSnapshot, &ps));
		CloseHandle(hSnapshot);
		return false;

	}

	//杀死指定进程
	bool ClientAutoUpdate::KillProcess1(DWORD dwPid)
	{
		//关闭进程
		HANDLE killHandle = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION |   // Required by Alpha
			PROCESS_CREATE_THREAD |   // For CreateRemoteThread
			PROCESS_VM_OPERATION |   // For VirtualAllocEx/VirtualFreeEx
			PROCESS_VM_WRITE,             // For WriteProcessMemory);
			FALSE, dwPid);
		if (killHandle == NULL)
			return false;
		TerminateProcess(killHandle, 0);
		return true;
	}

	

	//更新前对用户本地文件的备份
	//void ClientAutoUpdate::backup()
	//{
	//	auto localBackupIt = localCommonPath_md5.begin();
	//	std::string dirName = "./backup/";
	//	if (_access(dirName.c_str(), 0) == -1)
	//	{
	//		_mkdir(dirName.c_str());
	//	}


	//	while (localBackupIt != localCommonPath_md5.end())
	//	{
	//		int pos = 0;
	//		int length = localBackupIt->first.length();
	//		std::string subDirName;
	//		while ((pos = localBackupIt->first.find("/", pos)) != std::string::npos) {
	//			subDirName = localBackupIt->first.substr(0, pos + 1);
	//			dirName = "./backup/" + subDirName;
	//			if (_access(dirName.c_str(), 0) == -1)
	//			{
	//				_mkdir(dirName.c_str());
	//			}
	//			pos++;
	//		}

	//		std::string s = "./backup" + localBackupIt->first;
	//		CopyFileA(localBackupIt->first.c_str(), _T(s.c_str()), FALSE);
	//		localBackupIt++;
	//	}
	//}

	//执行增量更新
	void ClientAutoUpdate::partialUpdate()
	{
		try
		{
			auto localDeleteIt = localCommonPath_md5.begin();

			while (localDeleteIt != localCommonPath_md5.end())
			{
				DeleteFileA(localDeleteIt->first.c_str());
				localDeleteIt++;
			}
			if (localDeleteIt != localCommonPath_md5.end())
			{
				throw(1);
			}

			auto localCommonAddIt = requestCommonPath_md5.begin();
			std::string moveBefore;

			while (localCommonAddIt != requestCommonPath_md5.end())
			{
				moveBefore = "./DownloadCommonFile/" + localCommonAddIt->first;
				MoveFileA(moveBefore.data(), localCommonAddIt->first.data());
				localCommonAddIt++;
			}
			if (localCommonAddIt != requestCommonPath_md5.end())
			{
				throw(1);
			}

			/*auto localMergeAddIt = requestMergePath_md5.begin();


			while (localMergeAddIt != requestMergePath_md5.end())
			{


				localMergeAddIt++;
			}
			if (localMergeAddIt != requestMergePath_md5.end())
			{
				throw(1);
			}*/

			std::fstream fileOut, fileIn;
			std::string str;
			auto localMergeIt = localMergePath_md5.begin();
			auto requestMergeIt = requestMergePath_md5.begin();
			while (localMergeIt != localMergePath_md5.end())
			{
				while (requestMergeIt != requestMergePath_md5.end())
				{
					if (requestMergeIt->first == localMergeIt->first)
					{
						fileOut.open("./DownloadMergeFile/" + requestMergeIt->first, std::ios::in | std::ios::out);
						fileOut >> str;
						fileIn.open("../" + localMergeIt->first, std::ios::app);
						fileIn << str;
						fileOut.close();
						fileIn.close();
					}
					else
					{
						moveBefore = "./DownloadMergeFile/" + requestMergeIt->first;
						MoveFileA(moveBefore.data(), localMergeIt->first.data());
					}
				}
				localMergeIt++;
				requestMergeIt = requestMergePath_md5.begin();
			}

		}
		catch (int a)
		{
			err->show();
			recovery();
		}

	}

	//执行全量更新
	void ClientAutoUpdate::allUpdate()
	{
		WinExec("./DownloadSoft/full_app_installer.exe", SW_SHOWNORMAL);
	}

	//更新完毕，启动指定进程
	void ClientAutoUpdate::endUpdate()
	{
		/*std::string name = "./" + processStartAfterUpdate + ".exe";
		WinExec(name.c_str(), SW_SHOWNORMAL);*/

		std::ifstream file;
		file.open("./config.ini");
		std::string bufFromFile;
		std::string s = "processStartAfterUpdate:";
		int length = s.length();
		while (!file.eof()) {
			file >> bufFromFile;
			int lengthProcessStartAfterUpdate;
			if (bufFromFile.find("processStartAfterUpdate:") != std::string::npos)
			{
				lengthProcessStartAfterUpdate = bufFromFile.find("processStartAfterUpdate:");
				processStartAfterUpdate.assign(bufFromFile, length, lengthProcessStartAfterUpdate - length);
			}
		}
		std::string strProcessName = processStartAfterUpdate;
		TCHAR tszProcess[64] = { 0 };
		lstrcpy(tszProcess, _T(strProcessName.c_str()));
		//启动程序
		SHELLEXECUTEINFO shellInfo;
		memset(&shellInfo, 0, sizeof(SHELLEXECUTEINFO));
		shellInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		shellInfo.fMask = NULL;
		shellInfo.hwnd = NULL;
		shellInfo.lpVerb = NULL;
		shellInfo.lpFile = tszProcess;                      // 执行的程序名(绝对路径)
		shellInfo.lpParameters = NULL;
		shellInfo.lpDirectory = "../";
		shellInfo.nShow = SW_MINIMIZE;                      //SW_SHOWNORMAL 全屏显示这个程序
		shellInfo.hInstApp = NULL;
		ShellExecuteEx(&shellInfo);
	}


	//更新失败的版本回退
	void ClientAutoUpdate::recovery()
	{
		auto shouldDeleteFileIt = requestCommonPath_md5.begin();
		while (shouldDeleteFileIt != requestCommonPath_md5.end())
		{
			DeleteFileA(shouldDeleteFileIt->first.c_str());
			shouldDeleteFileIt++;
		}
		shouldDeleteFileIt = requestMergePath_md5.begin();
		while (shouldDeleteFileIt != requestMergePath_md5.end())
		{
			DeleteFileA(shouldDeleteFileIt->first.c_str());
			shouldDeleteFileIt++;
		}
		auto recoveryIt = localCommonPath_md5.begin();
		while (recoveryIt != localCommonPath_md5.end())
		{
			MoveFileA(strcat("./backup/", recoveryIt->first.c_str()), recoveryIt->first.c_str());
			recoveryIt++;
		}
	}



	QString ClientAutoUpdate::signUrl(const QString & urlPath)
	{
		int64_t delta = 1494245974L;
		const QString signSalt = "nuctech+salt";
		std::unordered_map<std::string, std::string> sign_member_map;
		int64_t current = QDateTime::currentDateTime().toSecsSinceEpoch();
		QString offsetStr = QString::number(current - delta);
		QString uuidStr = QUuid::createUuid().toString().remove("{")
			.remove("}").remove("-").toLower();

		QString filePath = urlPath;
		QUrl urlObj(urlPath);
		if (urlObj.isValid())
		{
			filePath = urlObj.path();
		}

		QString signString = QString("%1=%2&%3=%4").arg("a").arg(offsetStr).arg("b").arg(uuidStr);

		QCryptographicHash qch(QCryptographicHash::Md5);
		qch.addData(filePath.toUtf8());
		qch.addData(signSalt.toUtf8());

		qch.addData(offsetStr.toUtf8());
		qch.addData(uuidStr.toUtf8());

		QString sMd52 = qch.result().toHex().toLower();
		signString += QString("&%1=%2").arg("c").arg(sMd52);
		return signString;
	}

	//用户不更新时的弹窗选择
	void ClientAutoUpdate::stop()
	{
		if (!(QMessageBox::information(this, tr("Help"), tr("Do you really want exit ?"), tr("Yes"), tr("No"))))
		{
			QApplication* app;
			app->exit(0);
		}
	}




}