/*
 * rssi.h
 *
 *  Created on: Nov 18, 2016
 *      Author: RedCarrottt
 */

#ifndef RSSI_H_
#define RSSI_H_

void update_max_rssi(float x, float y);
int get_pos(int bulb_num, float* ret_px, float* ret_py);

#endif /* !defined(RSSI_H_) */
