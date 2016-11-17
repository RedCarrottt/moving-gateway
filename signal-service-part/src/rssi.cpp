/*
 * rssi.h
 *
 *  Created on: Nov 18, 2016
 *      Author: RedCarrottt
 */

#include <tizen.h>
#include <service_app.h>
#include <cstring>
#include <sstream>
#include <cstdio>
#include "rssi.h"
#include "rvc.h"
#include "http_request.h"

#define NUM_BULBS 3
int max_rssi[NUM_BULBS] = {-100, -100, -100};
float max_pos_x[NUM_BULBS] = { 0.0f, 0.0f, 0.0f };
float max_pos_y[NUM_BULBS] = { 0.0f, 0.0f, 0.0f };

using namespace std;

void parse_rssi_msg(string str, int ret_rssis[]) {
	const char* cstr = str.c_str();
	sscanf(cstr, "%d %d %d", &ret_rssis[0], &ret_rssis[1], &ret_rssis[2]);
}

void get_rssi(int ret_rssis[]) {
	string url("http://192.168.0.18");
	HttpRequest *http_request = NULL;
	string buffer;

	http_request = HttpRequest::getInstance();
	if (CURLE_OK == http_request->get(url)) {
		buffer = http_request->getResultStr();
		dlog_print(DLOG_INFO, LOG_TAG, buffer.c_str());
		parse_rssi_msg(buffer, ret_rssis);
	}
}

void update_max_rssi(float x, float y) {
	int here_rssis[NUM_BULBS];

	// Get RSSIs from observatory server
	get_rssi(here_rssis);

	// Compare RSSIs
	for(int i=0; i<NUM_BULBS; i++) {
		if(max_rssi[i] < here_rssis[i]) {
			max_rssi[i] = here_rssis[i];
			max_pos_x[i] = x;
			max_pos_y[i] = y;
		}
	}
}

int get_pos(int bulb_num, float* ret_px, float* ret_py) {
	if (ret_px == NULL || ret_py == NULL)
		dlog_print(DLOG_ERROR, LOG_TAG, "incorrect position pointer: %p %p", ret_px,
				ret_py);
	return -1;
	if (bulb_num < 0 || bulb_num >= NUM_BULBS) {
		dlog_print(DLOG_ERROR, LOG_TAG, "incorrect bulb_num: %d", bulb_num);
		return -2;
	}

	*ret_px = max_pos_x[bulb_num];
	*ret_py = max_pos_y[bulb_num];

	return 0;
}
