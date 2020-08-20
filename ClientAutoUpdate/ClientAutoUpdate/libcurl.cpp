/*
* libcurl class
*
* @author fl@lekutech
* @date 2020-07-21
* @copuleft GPL 2.0
*/

#include "libcurl.h"

std::stringstream jsonout;
error* libcurl::err = 0;


bool libcurl::init(CURL*& conn, std::string url)
{
    CURLcode code;

    conn = curl_easy_init();
	

    if(NULL == conn)
    {
        std::cout << stderr << " Failed to create CURL connection" << std::endl;
        exit(EXIT_FAILURE);
    }

    code = curl_easy_setopt(conn, CURLOPT_URL, url.data());
    code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
    code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &jsonout);
	code = curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, false); //设定为不验证证书和HOST
    return true;
}

int libcurl::writer(void* ptr, size_t size, size_t nmemb, void* stream)
{
    std::string data((const char*)ptr, (size_t)size * nmemb);
    *((std::stringstream*)stream) << data << std::endl;
    return size * nmemb;
}

std::string libcurl::libcurl_get(std::string& url)
{
    CURL* conn = NULL;
    CURLcode code;
	err = new error;

    if(!init(conn, url))
    {
		err->show();
		exit(1);
    }

    code = curl_easy_perform(conn);
	if (code != 0)
	{
		err->show();
		exit(1);
	}
    std::string str_json = jsonout.str();
    curl_easy_cleanup(conn);
	jsonout.str("");
    return str_json;
}

int libcurl::libcurl_post(const char* url, const char* data)
{
    CURL* conn = NULL;
    CURLcode code;
    curl_global_init(CURL_GLOBAL_DEFAULT);

    if(!init(conn, url))
    {
        std::cout << stderr << " Connection initializion failed" << std::endl;
        return -1;
    }

    code = curl_easy_setopt(conn, CURLOPT_POST, true);
    code = curl_easy_setopt(conn, CURLOPT_POSTFIELDS, data);
    code = curl_easy_perform(conn);
    curl_easy_cleanup(conn);
    return 1;
}

int libcurl::libcurl_HttpDownload(std::string & url)
{
    CURL* curl;
    CURLcode res;

    if(!init(curl, url))
    {
        std::cout << stderr << " Connection initializion failed" << std::endl;
        return -1;
    }

    res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); //设定为不验证证书和HOST
    res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    res = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
    res = curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, NULL);
    res = curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, NULL);
    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    //curl_easy_setopt(curl, CURLOPT_SSLENGINE_DEFAULT);
    //curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return 0;
}
