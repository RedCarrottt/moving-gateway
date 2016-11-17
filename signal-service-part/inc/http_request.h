/*
 * http_request.h
 *
 *  Created on: Nov 15, 2016
 *      Author: mini
 */
#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include <curl/curl.h>
#include <net_connection.h>
#include <string>
#include <dlog.h>
#include <json-glib/json-glib.h>
#include <json-glib/json-gobject.h>

using namespace std;

static size_t writeCb(char *buf, size_t size, size_t nmemb, void *up);

class HttpRequest{

public:
	static HttpRequest* getInstance();
	~HttpRequest();

	void post(string &url_ip, string &post_data);
	CURLcode get(string &url_ip);

	string getResultStr(void) const
	{
		return this->ret;
	}
private:
	static HttpRequest *http_request;
	string ret;
	HttpRequest();
	CURL *curl;
};

#endif /* HTTP_REQUEST_H_ */
