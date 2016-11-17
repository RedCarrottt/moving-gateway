#include <tizen.h>
#include <service_app.h>
#include <rvc_api.h>

#include "rvc.h"

/* RedCarrottt Start */
#include <unistd.h>
#include "pthread.h"
#include "rssi.h"

#define UPDATE_MAX_RSSI_PERIOD_US (1000 * 400) // 0.4s

pthread_t p_rssi_thread;

void *rssi_thread_func(void *data) {
	/* TODO: Replace it with present global position! */
	float pos_x, pos_y;
	pos_x = 1.0f, pos_y = 1.0f;

	while (true) {
		update_max_rssi(pos_x, pos_y);
		usleep(UPDATE_MAX_RSSI_PERIOD_US);
	}

	return NULL;
}
/* RedCarrottt End */

bool service_app_create(void *data) {
	if (rvc_initialize() != RVC_USER_ERROR_NONE) {
		return false;
	}
	/* RedCarrottt Start */
	int thread_id;
	thread_id = pthread_create(&p_rssi_thread, NULL, rssi_thread_func, NULL);
	if (thread_id < 0) {
		return false;
	}
	/* RedCarrottt End */

	// Todo: add your code here.
	return true;
}

void service_app_terminate(void *data) {
	rvc_deinitialize();

	/* RedCarrottt Start */
	int p_rssi_thread_status;
	pthread_join(p_rssi_thread, (void **) p_rssi_thread_status);
	/* RedCarrottt End */

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

static void service_app_low_memory(app_event_info_h event_info,
		void *user_data) {
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
