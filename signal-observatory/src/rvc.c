#include <tizen.h>
#include <service_app.h>
#include <rvc_api.h>

#include <app_control.h>
#include <bluetooth.h>
#include <glib.h>

#include "rvc.h"

void adapter_device_discovery_state_changed_cb(int result,
		bt_adapter_device_discovery_state_e discovery_state,
		bt_adapter_device_discovery_info_s *discovery_info, void* user_data) {
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"adapter_device_discovery_state_changed_cb()");
	if (result != BT_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG,
				"[adapter_device_discovery_state_changed_cb] failed! result(%d).",
				result);

		return;
	}
	GList** searched_device_list = (GList**) user_data;
	switch (discovery_state) {
	case BT_ADAPTER_DEVICE_DISCOVERY_STARTED:
		dlog_print(DLOG_DEBUG, LOG_TAG, "BT_ADAPTER_DEVICE_DISCOVERY_STARTED");
		break;
	case BT_ADAPTER_DEVICE_DISCOVERY_FINISHED:
		dlog_print(DLOG_DEBUG, LOG_TAG, "BT_ADAPTER_DEVICE_DISCOVERY_FINISHED");
		break;
	case BT_ADAPTER_DEVICE_DISCOVERY_FOUND:
		dlog_print(DLOG_DEBUG, LOG_TAG, "BT_ADAPTER_DEVICE_DISCOVERY_FOUND");
		if (discovery_info != NULL) {
			dlog_print(DLOG_DEBUG, LOG_TAG, "Device Address: %s",
					discovery_info->remote_address);
			dlog_print(DLOG_DEBUG, LOG_TAG, "Device Name is: %s",
					discovery_info->remote_name);
			dlog_print(DLOG_DEBUG, LOG_TAG, "RSSI : %d", discovery_info->rssi);
			bt_adapter_device_discovery_info_s * new_device_info = malloc(
					sizeof(bt_adapter_device_discovery_info_s));
			if (new_device_info != NULL) {
				memcpy(new_device_info, discovery_info,
						sizeof(bt_adapter_device_discovery_info_s));
				new_device_info->remote_address = strdup(
						discovery_info->remote_address);
				new_device_info->remote_name = strdup(
						discovery_info->remote_name);
				*searched_device_list = g_list_append(*searched_device_list,
						(gpointer) new_device_info);
			}
		}
		break;
	}
}

GList *devices_list = NULL;

int discover_bt_anyway(void) {
	int ret;

	dlog_print(DLOG_ERROR, LOG_TAG,
					"discover_bt_anyway() enter");

	ret = bt_adapter_set_device_discovery_state_changed_cb(
			adapter_device_discovery_state_changed_cb, (void*) &devices_list);

	if (ret != BT_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG,
				"[bt_adapter_set_device_discovery_state_changed_cb] failed.");
	}

	ret = bt_adapter_start_device_discovery();
	return ret;
}

int discover_bt_if_turned_on(void) {
	bt_adapter_state_e adapter_state;

	dlog_print(DLOG_ERROR, LOG_TAG,
						"discover_bt_if_turned_on() enter");

	// Check whether the Bluetooth adapter is enabled
	int ret = bt_adapter_get_state(&adapter_state);
	if (ret != BT_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[bt_adapter_get_state] failed");
		return -1;
	}

	// If the Bluetooth adapter is enabled, start to discover Bluetooth devices
	if (adapter_state == BT_ADAPTER_ENABLED) {
		ret = discover_bt_anyway();
		return 0;
	} else {
		dlog_print(DLOG_ERROR, LOG_TAG, "bluetooth adapter not enabled: %x", adapter_state);
		return -2;
	}
}

// Callback: Adapter State Changed
//void adapter_state_changed_cb(int result, bt_adapter_state_e adapter_state,
//		void* user_data) {
//	if (result != BT_ERROR_NONE) {
//		dlog_print(DLOG_ERROR, LOG_TAG,
//				"[adapter_state_changed_cb] failed! result=%d", result);
//
//		return;
//	}
//	if (adapter_state == BT_ADAPTER_ENABLED) {
//		dlog_print(DLOG_DEBUG, LOG_TAG,
//				"[adapter_state_changed_cb] Bluetooth is enabled!");
//
//		/* Get information about Bluetooth adapter */
//		char *local_address = NULL;
//		bt_adapter_get_address(&local_address);
//		dlog_print(DLOG_DEBUG, LOG_TAG,
//				"[adapter_state_changed_cb] Adapter address: %s.",
//				local_address);
//		if (local_address)
//			free(local_address);
//		char *local_name;
//		bt_adapter_get_name(&local_name);
//		dlog_print(DLOG_DEBUG, LOG_TAG,
//				"[adapter_state_changed_cb] Adapter name: %s.", local_name);
//		if (local_name)
//			free(local_name);
//		/* Visibility mode of the Bluetooth device */
//		bt_adapter_visibility_mode_e mode;
//		/* Duration until the visibility mode is changed so that other devices cannot find your device */
//		int duration = 1;
//		bt_adapter_get_visibility(&mode, &duration);
//		switch (mode) {
//		case BT_ADAPTER_VISIBILITY_MODE_NON_DISCOVERABLE:
//			dlog_print(DLOG_DEBUG, LOG_TAG,
//					"[adapter_state_changed_cb] Visibility: NON_DISCOVERABLE");
//			break;
//		case BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE:
//			dlog_print(DLOG_DEBUG, LOG_TAG,
//					"[adapter_state_changed_cb] Visibility: GENERAL_DISCOVERABLE");
//			break;
//		case BT_ADAPTER_VISIBILITY_MODE_LIMITED_DISCOVERABLE:
//			dlog_print(DLOG_DEBUG, LOG_TAG,
//					"[adapter_state_changed_cb] Visibility: LIMITED_DISCOVERABLE");
//			break;
//		}
//
//		// If the Bluetooth adapter is enabled, start to discover Bluetooth devices
////		discover_bt_if_turned_on();
//	} else {
//		dlog_print(DLOG_DEBUG, LOG_TAG,
//				"[adapter_state_changed_cb] Bluetooth is disabled!");
//		/*
//		 When you try to get device information
//		 by invoking bt_adapter_get_name(), bt_adapter_get_address(), or bt_adapter_get_visibility(),
//		 BT_ERROR_NOT_ENABLED occurs
//		 */
//	}
//}

//void set_bt_state_changed_cb() {
//	int ret = bt_adapter_set_state_changed_cb(adapter_state_changed_cb, NULL);
//	if (ret != BT_ERROR_NONE)
//		dlog_print(DLOG_ERROR, LOG_TAG,
//				"[bt_adapter_set_state_changed_cb()] failed.");
//}

int inititalize_bt(void) {
	int ret = 0;
	ret = bt_initialize();
	if (ret != BT_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "[bt_initialize] failed.");
		return -1;
	}
	return 0;
}

int turn_on_bt(void) {
	int ret = 0;

	app_control_h service = NULL;

	app_control_create(&service);
	if (service == NULL) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "service_create failed!\n");

		return 0;
	}
	app_control_set_operation(service,
			"APP_CONTROL_OPERATION_SETTING_BT_ENABLE");
	ret = app_control_send_launch_request(service, NULL, NULL);

	app_control_destroy(service);
	if (ret == APP_CONTROL_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "Succeeded to Bluetooth On/Off app!\n");

		return 0;
	} else {
		dlog_print(DLOG_DEBUG, LOG_TAG,
				"Failed to relaunch Bluetooth On/Off app! %x\n", ret);

		return -1;
	}

	return 0;
}

bool service_app_create(void *data) {
	if (rvc_initialize() != RVC_USER_ERROR_NONE) {
		return false;
	}
	dlog_print(DLOG_DEBUG, LOG_TAG, "Service launched!!");

	// Turn on Bluetooth
	inititalize_bt();
	turn_on_bt();
//	set_bt_state_changed_cb();

// Discover bluetooth adapter if Bluetooth adapter has already enabled
//	discover_bt_if_turned_on();
	discover_bt_anyway();

	return true;
}

void service_app_terminate(void *data) {
	rvc_deinitialize();

	// Todo: add your code here.
	return;
}

void service_app_control(app_control_h app_control, void *data) {
	// Todo: add your code here.
	return;
}

static void service_app_lang_changed(app_event_info_h event_info,
		void *user_data) {
	/*APP_EVENT_LANGUAGE_CHANGED*/
	return;
}

static void service_app_region_changed(app_event_info_h event_info,
		void *user_data) {
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void service_app_low_battery(app_event_info_h event_info,
		void *user_data) {
	/*APP_EVENT_LOW_BATTERY*/
}

static void service_app_low_memory(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LOW_MEMORY*/
}

int main(int argc, char* argv[]) {
	char ad[50] = { 0, };
	service_app_lifecycle_callback_s event_callback;
	app_event_handler_h handlers[5] = { NULL, };

	event_callback.create = service_app_create;
	event_callback.terminate = service_app_terminate;
	event_callback.app_control = service_app_control;

	service_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, service_app_low_battery, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, service_app_low_memory, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, service_app_lang_changed, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, service_app_region_changed, &ad);

	return service_app_main(argc, argv, &event_callback, ad);
}
