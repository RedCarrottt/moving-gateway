#ifndef HUE_H_
#define HUE_H_
#define HUE_JSON_SIZE 128
#include "http_request.h"
#include <list>
using namespace std;
static const char* hue_bridge_ip= "192.168.0.10";
static const char* auth_key = "KNzSpi9sRULmeLOpukfhc0Ffwzsy2PNMdtx1di23";
static const char *hue_json_object =
"{"
	 "\"on\":%d,"
     "\"bri\":%d,"
	 "\"sat\":%d,"
"}";

class Hue{	
	void put(void);
	void setOn(bool _on)
	{
		this->on = _on;
		this->put();
	}
	void setBri(int _bri)
	{
		this->bri = _bri;
		this->put();
	}
	void setSat(int _sat)
	{
		this->sat = _sat;
		this->put();
	}
	void doubleXY(double x, double y)
	{
		this->xy[0] = x;
		this->xy[1] = y;
		this->put();
	}
public:
	string uniqueid;
 	bool on;
	int bri;
	int sat;
	float xy[2];
	int id;
};

void get_hue_list(void);
string charToString(const char* _str);
string makeUrlStr(void);
void is_check_duplicated(Hue* hue);
Hue* find_hue(string uniqueid);
string int_to_string(int i);
string make_url_str_with_num(int i);
static list<Hue*> hue_list;
void print_list(void);

#endif /* HUE_H_ */
