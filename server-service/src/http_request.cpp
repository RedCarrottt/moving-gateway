/*
 * http_request.cpp
 *
 *  Created on: Nov 15, 2016
 *      Author: mini
 */
#include "http_request.h"

HttpRequest *HttpRequest::http_request = NULL;

HttpRequest *HttpRequest::getInstance()
{
	if(http_request == NULL)
		http_request = new HttpRequest();
	return http_request;
}

static size_t writeCb(char *buf, size_t size, size_t nmemb, void *up)
{
	for(int c = 0; c < size*nmemb; c++)
		((string*)up)->push_back(buf[c]);
	return  size * nmemb;
}

CURLcode HttpRequest::get(string &url_ip)
{
	string ret;
	this->curl = curl_easy_init();
	CURLcode code(CURLE_FAILED_INIT);
		if(curl){
			if(CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url_ip.c_str()))
			&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCb))
			&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &(this->ret)))){
				code = curl_easy_perform(curl);
			}
			curl_easy_cleanup(this->curl);
	}
	else
		dlog_print(DLOG_INFO, "MyTag", "curl is error");

	return code;
}

void HttpRequest::post(string &url_ip, string &post_data)
{
	this->curl = curl_easy_init();
	CURLcode code(CURLE_FAILED_INIT);
	if(!(url_ip.empty()) && !(post_data.empty()) && curl){
		if(CURLE_OK == (code = curl_easy_setopt(this->curl, CURLOPT_URL, url_ip.c_str()))
		&& CURLE_OK == (code = curl_easy_setopt(this->curl, CURLOPT_POST, 1))
		&& CURLE_OK == (code = curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, post_data.c_str()))){
			curl_easy_perform(curl);
		}
		curl_easy_cleanup(curl);
	}
	else
		dlog_print(DLOG_INFO, "MyTag", "url_ip is empty");
}

HttpRequest::HttpRequest()
{
	curl_global_init(CURL_GLOBAL_ALL);
	curl = NULL;
}

HttpRequest::~HttpRequest()
{
	curl_easy_cleanup(curl);
	curl = NULL;
}