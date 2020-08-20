/*
* libcurl class
*
* @author fl@lekutech
* @date 2020-07-21
* @copuleft GPL 2.0
*/

#ifndef __LIBCURL_H__
#define __LIBCURL_H__

#include <string>
#include <iostream>
#include <sstream>

#include "error.h"
#include "curl/curl.h"
#include "ClientAutoUpdate.h"

class libcurl
{
        //��̬��Ա����ֻ�ܷ��ʾ�̬��Ա�����;�̬��Ա����
    public:
        static int writer(void* ptr, size_t size, size_t nmemb, void* stream);
        static bool init(CURL* &, std::string);
        //http get ����
        static std::string libcurl_get(std::string& url);
        //http post����
        static int libcurl_post(const char* url, const char* data);
        //http����
        static int libcurl_HttpDownload(std::string& url);
 private:
	 static error*  err;
};

#endif