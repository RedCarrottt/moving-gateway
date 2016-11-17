/*
 * hue.cpp
 *
 *  Created on: Nov 17, 2016
 *      Author: mini
 */

#include "hue.h"
#include <cstring>
#include <sstream>
using namespace std;
string charToString(const char* _str)
{
	std::string tmp = _str;
	return tmp;
}
string make_url_str_with_num(int i)
{
	string url;
	url += "http://";
	url += charToString(hue_bridge_ip);
	url += "/api/";
	url += charToString(auth_key);
	url += "/lights/";
	url += int_to_string(i);
	url += "/state";
	return url;
}

string makeUrlStr(void)
{
	string url;
	url += "http://";
	url += charToString(hue_bridge_ip);
	url += "/api/";
	url += charToString(auth_key);
	url += "/lights/";
	dlog_print(DLOG_INFO, "MyType", url.c_str());
	return url;
}

static void parse_members(JsonObject* object, const gchar *member_name, JsonNode *member_node, gpointer user_data)
{
	Hue* hue = (Hue*)user_data;
	if(g_strcmp0(member_name, "error") == 0)
	{
		delete hue;
		hue = NULL;
		return;
	}
	else if(g_strcmp0(member_name, "state") == 0)
	{
		JsonObject* obj = json_node_get_object(member_node);
		JsonArray* xy;
		hue->on = (bool)json_node_get_boolean(json_object_get_member(obj, "on"));
		hue->bri = (int)json_node_get_int(json_object_get_member(obj, "bri"));
		hue->sat = (int)json_node_get_int(json_object_get_member(obj, "sat"));
		xy = (JsonArray*)json_node_get_array(json_object_get_member(obj, "xy"));
	}
	else if(g_strcmp0(member_name, "uniqueid") == 0)
	{
		string temp(json_node_get_string(member_node));
		hue->uniqueid = temp;
		dlog_print(DLOG_INFO, "MyTag", "Unique-ID :%s\n", temp.c_str());
	}
}

static void parse_get_json_cmd(string &cmd, Hue *hue)
{
	JsonParser *jsonParser = NULL;
	GError *error = NULL;
	jsonParser = json_parser_new();

	if(jsonParser != NULL)
	{
		if(json_parser_load_from_data(jsonParser, cmd.c_str(), -1, &error))
		{
			JsonNode *root = NULL;
			JsonObject *object = NULL;
			root = json_parser_get_root(jsonParser);

			if(JSON_NODE_TYPE(root) == JSON_NODE_OBJECT){
				object = json_node_get_object(root);
				if(object != NULL)
				{
					json_object_foreach_member(object, parse_members, NULL);
				}
			}
		}
		if(error != NULL)
			g_error_free(error);
		if(jsonParser != NULL)
			g_object_unref(jsonParser);
	}
}
void print_list(void)
{
	list<Hue*>::iterator iter;
	for(iter=hue_list.begin(); iter != hue_list.end(); ++iter)
	{
		dlog_print(DLOG_INFO, "MyTag", "List : %s", (*iter)->uniqueid);
	}
}
void is_check_duplicated(Hue* hue)
{
	bool check_all=true;
	list<Hue*>::iterator iter;

	for(iter=hue_list.begin(); iter != hue_list.end(); ++iter)
	{
		string temp = (string)((*iter)->uniqueid);
		if(temp.compare(hue->uniqueid) == 0)
			check_all = false;
	}
	if(!check_all)
		hue_list.push_back(hue);
}
Hue* find_hue(string uniqueid)
{
	list<Hue*>::iterator iter;

	for(iter=hue_list.begin(); iter != hue_list.end(); ++iter)
	{
		string temp = (string)((*iter)->uniqueid);
		if(temp.compare(uniqueid) == 0)
				return *iter;
	}
	return NULL;
}

string int_to_string(int i)
{
	stringstream ss;
	ss << i;
	string str = ss.str();
	return str;
}

void get_hue_list(void)
{
	HttpRequest *http_request = NULL;
	Hue *tmp_hue = NULL;
	string url;
	string url_specific_hue;
	string buffer;
	url = makeUrlStr();
	for(int i=1; i<=3; i++){
		tmp_hue = new Hue();
		url_specific_hue = url + int_to_string(i);
		dlog_print(DLOG_INFO, "MyType", "%d : URL - %s", i, url_specific_hue.c_str());
		http_request =  HttpRequest::getInstance();
		    if(CURLE_OK == http_request->get(url_specific_hue)){
		    	buffer = http_request->getResultStr();
		    	dlog_print(DLOG_INFO, "MyTag", buffer.c_str());
		    	parse_get_json_cmd(buffer, tmp_hue);
		    }
		    else{
		    	delete tmp_hue;
		    	tmp_hue = NULL;
		    }
		    if(!tmp_hue){
		    	tmp_hue->id = i;
		    	is_check_duplicated(tmp_hue);
		    }
	}
    //json parse
}

void Hue::put(void)
{
	HttpRequest *http_request = NULL;
	char msg[HUE_JSON_SIZE+1]= {0,};
	string url;
	url = make_url_str_with_num(this->id);
	snprintf(msg, HUE_JSON_SIZE, hue_json_object, this->on, this->bri, this->sat);
	string body(msg);
	dlog_print(DLOG_INFO, "MyTag", body.c_str());
	http_request = HttpRequest::getInstance();
	http_request->post(url, body);
}






