#include <tizen.h>
#include <service_app.h>
#include <rvc_api.h>


#include "rvc.h"


/*
 *  Position Variables: g_pos, g_pre_pos, g_cur_pos
 *  Because the position value of RVC resets when mode switches,
 *  we need to record the previous values and update current value,
 *  And lastly global position value.
 */

float g_pos_x, g_pos_y, g_pos_q;		// Global position
float pre_pos_x, pre_pos_y, pre_pos_q;	// Previous position values
float cur_pos_x, cur_pos_y, cur_pos_q;	// Current position values


/**
* The following code shows how to register the rvc callback.
* @code

void rvc_mode_callback(rvc_mode_type_get_e mode, void* data){
	// Todo: add your code here.
}

void rvc_error_callback(rvc_device_error_type_e error, void* data){
	// Todo: add your code here.
}

void rvc_wheel_callback(signed short wheel_vel_left, signed short wheel_vel_right, void* data){
	// Todo: add your code here.
}

void rvc_pose_callback(float pose_x, float pose_y, float pose_q, void* data){
	// Todo: add your code here.
}

void rvc_bumper_callback(unsigned char bumper_left, unsigned char bumper_right, void* data){
	// Todo: add your code here.
}

void rvc_cliff_callback(unsigned char cliff_left, unsigned char cliff_center, unsigned char cliff_right, void* data){
	// Todo: add your code here.
}

void rvc_lift_callback(unsigned char lift_left, unsigned char lift_right, void* data){
	// Todo: add your code here.
}

void rvc_magnet_callback(unsigned char magnet, void* data){
	// Todo: add your code here.
}

void rvc_suction_callback(rvc_suction_state_e state, void* data){
	// Todo: add your code here.
}

//void rvc_batt_callback(rvc_batt_level_e level, void* data){
//	// Todo: add your code here.
//}

void rvc_voice_callback(rvc_voice_type_e type, void* data){
	// Todo: add your code here.
}

void rvc_batt_low_callback(void* data){
	// Todo: add your code here.
}

void rvc_register_callback(){
	rvc_set_mode_evt_cb(rvc_mode_callback, NULL);
	rvc_set_error_evt_cb(rvc_error_callback, NULL);
	rvc_set_wheel_vel_evt_cb(rvc_wheel_callback, NULL);
	rvc_set_pose_evt_cb(rvc_pose_callback, NULL);
	rvc_set_bumper_evt_cb(rvc_bumper_callback, NULL);
	rvc_set_cliff_evt_cb(rvc_cliff_callback, NULL);
	rvc_set_lift_evt_cb(rvc_lift_callback, NULL);
	rvc_set_magnet_evt_cb(rvc_magnet_callback, NULL);
	rvc_set_suction_evt_cb(rvc_suction_callback, NULL);
	rvc_set_batt_evt_cb(rvc_batt_callback, NULL);
	rvc_set_voice_evt_cb(rvc_voice_callback, NULL);
	rvc_set_batt_low_evt_cb(rvc_batt_low_callback, NULL);
}

void rvc_unregister_callback(){
	rvc_unset_mode_evt_cb();
	rvc_unset_error_evt_cb();
	rvc_unset_wheel_vel_evt_cb();
	rvc_unset_pose_evt_cb();
	rvc_unset_bumper_evt_cb();
	rvc_unset_cliff_evt_cb();
	rvc_unset_lift_evt_cb();
	rvc_unset_magnet_evt_cb();
	rvc_unset_suction_evt_cb();
	rvc_unset_batt_evt_cb();
	rvc_unset_voice_evt_cb();
	rvc_unset_batt_low_evt_cb();
}
* @endcode
*/

/**
* The following code shows how to get the rvc info.
* @code
void get_rvc_info(){
    rvc_mode_type_get_e mode;
	rvc_get_mode(&mode);

	rvc_device_error_type_e error;
	rvc_get_error(&error);

	signed short wheel_vel_left, wheel_vel_right;
	rvc_get_wheel_vel(&wheel_vel_left, &wheel_vel_right);

	unsigned char bumper_left, bumper_right;
	rvc_get_bumper(&bumper_left, &bumper_right);

	float pose_x, pose_y, pose_q;
	rvc_get_pose(&pose_x, &pose_y, &pose_q);

	unsigned char cliff_left, cliff_center, cliff_right;
	rvc_get_cliff(&cliff_left, &cliff_center, &cliff_right);

	unsigned char lift_left, lift_right;
	rvc_get_lift(&lift_left, &lift_right);

	unsigned char magnet;
	rvc_get_magnet(&magnet);

	rvc_suction_state_e suction;
	rvc_get_suction_state(&suction);

	unsigned char is_on, hour, minute;
	rvc_get_reserve(RVC_RESERVE_TYPE_ONCE, &is_on, &hour, &minute);
	rvc_get_reserve(RVC_RESERVE_TYPE_DAILY, &is_on, &hour, &minute);

	float lin_vel, ang_vel;
	rvc_get_lin_ang_vel(&lin_vel, &ang_vel);

	rvc_batt_level_e level;
	rvc_get_battery_level(&level);

	rvc_voice_type_e type;
	rvc_get_voice_type(&type);
}
* @endcode
*/

/**
* The following code shows how to set command the rvc.
* @code
void set_rvc_command(){
    rvc_set_mode(RVC_MODE_SET_PAUSE);
    rvc_set_mode(RVC_MODE_SET_DOCKING);
    rvc_set_mode(RVC_MODE_SET_CLEANING_AUTO);
    rvc_set_mode(RVC_MODE_SET_CLEANING_SPOT);

    rvc_set_time(23, 00);

    rvc_set_voice(RVC_VOICE_TYPE_BEEP);
    rvc_set_voice(RVC_VOICE_TYPE_NONE);
    rvc_set_voice(RVC_VOICE_TYPE_WOMAN);
    rvc_set_voice(RVC_VOICE_TYPE_MAN);

    rvc_set_suction_state(RVC_SUCTION_SLIENT);
    rvc_set_suction_state(RVC_SUCTION_NORMAL);
    rvc_set_suction_state(RVC_SUCTION_TURBO);

    rvc_set_reserve(RVC_RESERVE_TYPE_ONCE, 23, 00);
    rvc_set_reserve(RVC_RESERVE_TYPE_DAILY, 23, 00);

    rvc_set_reserve_cancel(RVC_RESERVE_TYPE_ONCE);
    rvc_set_reserve_cancel(RVC_RESERVE_TYPE_DAILY);

    for(int i = 0 ; i < 100; i++){
        rvc_set_control(RVC_CONTROL_DIR_FORWARD);
        sleep(50000);
	}

    for(int i = 0 ; i < 100; i++){
        rvc_set_control(RVC_CONTROL_DIR_LEFT);
        sleep(50000);
	}

	for(int i = 0 ; i < 100; i++){
        rvc_set_control(RVC_CONTROL_DIR_RIGHT);
        sleep(50000);
	}

	for(int i = 0 ; i < 100; i++){
        rvc_set_wheel_vel(2, 2);
        sleep(50000);
	}

    for(int i = 0 ; i < 100; i++){
        rvc_set_lin_ang(10.f, 0.f);
        sleep(50000);
	}
}
* @endcode
*/


/*
 *  This function changes the direction of the RVC.
 *  This function
 */
void change_direction(float q){
	dlog_print(DLOG_DEBUG, LOG_TAG, "[FUNCTION] change direction function");
	rvc_mode_type_get_e mode;
	rvc_get_mode(&mode);

	// When mode is already in manual mode,
	// No need to change mode.
	if(mode == RVC_MODE_GET_MANUAL){

	}
	// When mode is not in manual mode,
	// It requires some time to change mode.
	else {
		// Call control function to changed the mode to manual
		rvc_set_control(RVC_CONTROL_DIR_RIGHT);
	}

	int _switch = 1;
	float range = 0.1;
	while(_switch){
		float delta = q - cur_pos_q;
		if(delta < -range){
			rvc_set_control(RVC_CONTROL_DIR_RIGHT);
		}
		else if(delta > range){
			rvc_set_control(RVC_CONTROL_DIR_LEFT);
		}
		else {
			_switch = 0;
		}
	}
}

/*
 *  This function moves RVC to the specific spot (X,Y).
 *  (X,Y) is the absolute value in global position coordinate.
 *  This requires setting direction to the goal and
 *  Moving to the goal avoiding the obstacles.
 */
int _finish = 0;
void move_to_xy(float x, float y){
	dlog_print(DLOG_DEBUG, LOG_TAG, "[FUNCTION] move_to_xy function start");
	_finish = 0;
	while(!_finish){
		float dir;					// Direction to head to (radian)
		float slope = (y - g_pos_y)/(x - g_pos_x);

		// The range is [-PI/2, PI/2]
		dir = atan(slope);

		// When the range is over [-PI/2, PI/2],
		// which is the range of input to tangent,
		if((x - g_pos_x) < 0){
			dir = dir + M_PI;
		}

		// Make the direction in the range of [-PI, PI]
		if(dir > M_PI){
			dir -= 2 * M_PI;
		}
		else if(dir < -M_PI){
			dir += 2 * M_PI;
		}

		change_direction(dir);
		rvc_set_control(RVC_CONTROL_DIR_FORWARD);

		// If it meets obstacle,


		// If it reaches the goal point
		float delta_x = x - g_pos_x;
		float delta_y = y - g_pos_y;
		float range = 50.0;
		if( (fabsf(delta_x) < range) && (fabsf(delta_y) < range) ){
			dlog_print(DLOG_DEBUG, LOG_TAG, "Move to function finished");
			_finish = 1;
			rvc_set_mode(RVC_MODE_SET_PAUSE);
		}
	}


}

/*
 *  This function is executed by pthread,
 *  which was created at the app_create stage.
 */
void *t_function(void* data){
	dlog_print(DLOG_DEBUG, LOG_TAG, "Start thread function");


	move_to_xy(-200.0, -200.0);
	move_to_xy(-400.0, -400.0);
	move_to_xy(-100.0, -200.0);

}

/*
 *  Rotating matrix is
 *  ( cos(*) -sin(*)
 *    sin(*) cos(*) )
 */
void rotate_value_with_q(float x, float y, float theta, float *out_x, float *out_y){
	*out_x = x * cos(theta) - y * sin(theta);
	*out_y = x * sin(theta) + y * cos(theta);
}

/*
 *  Position changed callback need to record the current position,
 *  because when mode changes, the value resets.
 *  Therefore, we use 'cur_pos' value instead of 'pose' value in this callback.
 *  In addition, It needs to update global position.
 */
void my_pose_changed_cb(float pose_x, float pose_y, float pose_q, void* user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "POSE: %f,%f,%f", pose_x, pose_y, pose_q);

	// When Mode changed from Idle to Manual, record the position value.
	// (Because, the position value is reset.)
	// PRE is the value of position before position value reset.
	// CUR is the value of position after position value changed.
	// ****** This function should be executed before update "cur_pos" value ********
	if(pose_x < 5.0 && pose_x > -5.0 && pose_y < 5.0 && pose_y > -5.0 ){
		float changed_x, changed_y;
		rotate_value_with_q(cur_pos_x, cur_pos_y, pre_pos_q, &changed_x, &changed_y);
		pre_pos_x += changed_x;
		pre_pos_y += changed_y;
		pre_pos_q += cur_pos_q;
	}

	// Current position data could be reset. (unstable)
	// Therefore, we record the current position.
	cur_pos_x = pose_x;
	cur_pos_y = pose_y;
	cur_pos_q = pose_q;

	// Current direction is already affected by previous PI.
	// Therefore, we need to rotate the position (x,y)
	// with the amount of previous q.
	float changed_x, changed_y;
	rotate_value_with_q(cur_pos_x, cur_pos_y, pre_pos_q, &changed_x, &changed_y);
	g_pos_x = pre_pos_x + changed_x;
	g_pos_y = pre_pos_y + changed_y;

	// Global Direction PI = Previous PI + Current PI
	g_pos_q = pre_pos_q + cur_pos_q;

	dlog_print(DLOG_DEBUG, LOG_TAG, "global: %f,%f,%f", g_pos_x, g_pos_y, g_pos_q);

}

void my_mode_changed_cb(rvc_mode_type_get_e mode, void* user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Mode: %d", mode);

	// Print current mode.
	switch (mode){
		case RVC_MODE_GET_IDLE:
			dlog_print(DLOG_DEBUG, LOG_TAG, "Mode: IDLE");
			break;
		case RVC_MODE_GET_CHARGE:
			dlog_print(DLOG_DEBUG, LOG_TAG, "Mode: CHARGING");
			break;
		case RVC_MODE_GET_PAUSE:
			dlog_print(DLOG_DEBUG, LOG_TAG, "Mode: PAUSE");
			break;
		case RVC_MODE_GET_ERROR:
			dlog_print(DLOG_DEBUG, LOG_TAG, "Mode: ERROR");
			break;
		case RVC_MODE_GET_SET_CLOCK:
			dlog_print(DLOG_DEBUG, LOG_TAG, "Mode: SET CLOCK");
			break;
		case RVC_MODE_GET_SET_RESERVE:
			dlog_print(DLOG_DEBUG, LOG_TAG, "Mode: SET RESERVE");
			break;
		case RVC_MODE_GET_DOCKING:
			dlog_print(DLOG_DEBUG, LOG_TAG, "Mode: DOCKING");
			break;
		case RVC_MODE_GET_CLEANING_AUTO:
			dlog_print(DLOG_DEBUG, LOG_TAG, "Mode: CLEANING AUTO");
			break;
		case RVC_MODE_GET_CLEANING_SPOT:
			dlog_print(DLOG_DEBUG, LOG_TAG, "Mode: CLEANING SPOT");
			break;
		case RVC_MODE_GET_MANUAL:
			dlog_print(DLOG_DEBUG, LOG_TAG, "Mode: MANUAL");
			break;
		case RVC_MODE_GET_POINTCLEANING:
			dlog_print(DLOG_DEBUG, LOG_TAG, "Mode: POINT CLEANING");
			break;
		case RVC_MODE_GET_ETC:
			dlog_print(DLOG_DEBUG, LOG_TAG, "Mode: ETC");
			break;
		default:
			break;
	}



}

void my_bumper_cb(unsigned char bumper_left, unsigned char bumper_right, void* data){
	static char prev_bump_left = 0;
	static char prev_bump_right = 0;

	if (bumper_left || bumper_right) {
		_finish = 1;
		rvc_set_mode(RVC_MODE_SET_PAUSE);
	}
	/*
	//Left bumper event!
	if (prev_bump_left != bumper_left && bumper_left == 1)
		rvc_set_mode(RVC_MODE_SET_PAUSE);
	//Right bumper event!
	else if (prev_bump_right != bumper_right && bumper_right ==1)
		rvc_set_mode(RVC_MODE_SET_PAUSE);
		*/

	prev_bump_left = bumper_left;
	prev_bump_right = bumper_right;
}

void my_error_cb(rvc_device_error_type_e error, void* data){
	dlog_print(DLOG_DEBUG, LOG_TAG, "error event occurred: %d", error);
}

void my_wheel_cb(signed short wheel_vel_left, signed short wheel_vel_right, void* data){
	dlog_print(DLOG_DEBUG, LOG_TAG, "wheel callback event occurred!");

}

void my_cliff_cb(unsigned char cliff_left, unsigned char cliff_center, unsigned char cliff_right, void* data){
	// Todo: add your code here.
}

void my_lift_cb(unsigned char lift_left, unsigned char lift_right, void* data){
	// Todo: add your code here.
}

void my_magnet_cb(unsigned char magnet, void* data){
	// Todo: add your code here.
}

void initialize_position(){
	// Initialize global position
	g_pos_x = g_pos_y = g_pos_q = 0.0;

	// Initialize previous position
	pre_pos_x = pre_pos_y = pre_pos_q = 0.0;

	// Initialize current position
	cur_pos_x = cur_pos_y = cur_pos_q = 0.0;
}

bool service_app_create(void *data)
{
	if(rvc_initialize()!=RVC_USER_ERROR_NONE)
	{
		return false;
	}

	// Todo: add your code here.
	dlog_print(DLOG_DEBUG, LOG_TAG, "Initialize position");
	initialize_position();

	rvc_set_suction_state(RVC_SUCTION_SLIENT);

	rvc_set_pose_evt_cb(my_pose_changed_cb, NULL);
	rvc_set_mode_evt_cb(my_mode_changed_cb, NULL);
	rvc_set_bumper_evt_cb(my_bumper_cb, NULL);
	rvc_set_error_evt_cb(my_error_cb, NULL);
	rvc_set_wheel_vel_evt_cb(my_wheel_cb, NULL);
	rvc_set_cliff_evt_cb(my_cliff_cb, NULL);
	rvc_set_lift_evt_cb(my_lift_cb, NULL);
	rvc_set_magnet_evt_cb(my_magnet_cb, NULL);

	// Make pthread
	pthread_t thread;
	int err;
	err = pthread_create(&thread, NULL, t_function, (void*) NULL);
	if(err < 0)
	{
		perror("thread create error!");
	}

	return true;
}

void service_app_terminate(void *data)
{
	rvc_deinitialize();

    // Todo: add your code here.
    return;
}

void service_app_control(app_control_h app_control, void *data)
{
    // Todo: add your code here.
	dlog_print(DLOG_DEBUG, LOG_TAG, "App Control Callback!");
    return;
}

static void
service_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	return;
}

static void
service_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/

}

static void
service_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
	dlog_print(DLOG_DEBUG, LOG_TAG, "Low Battery");
}

static void
service_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
	dlog_print(DLOG_DEBUG, LOG_TAG, "Low Memory");
}

int main(int argc, char* argv[])
{
    char ad[50] = {0,};
	service_app_lifecycle_callback_s event_callback;
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = service_app_create;
	event_callback.terminate = service_app_terminate;
	event_callback.app_control = service_app_control;

	service_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, service_app_low_battery, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, service_app_low_memory, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, service_app_lang_changed, &ad);
	service_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, service_app_region_changed, &ad);

	return service_app_main(argc, argv, &event_callback, ad);
}
