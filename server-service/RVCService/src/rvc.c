#include <tizen.h>
#include <rvc_api.h>

#include "rvc.h"

//json packet size
#define RVC_JSON_SIZE 512
#define ESLAB_PI ((float)3.14)

//server port number
#define RVC_SERVER_PORT 5000

//json format
static const char *rvc_pos_object_format =
"{"
  "\"position\":"
  "{"
    "\"x\":%f,"
   "\"y\":%f,"
   "\"q\":%f"
  "}"
"}";

/**
* This struct has tx information.
*/
typedef struct{
   int mode;
   int error;
   int bumper_left;
   int bumper_right;
   int cliff_left;
   int cliff_center;
   int cliff_right;
   int lift_left;
   int lift_right;
   int magnet;
   int suction;
   int voice;
   int battery;
   int once_on;
   int once_hour;
   int once_minute;
   int daily_on;
   int daily_hour;
   int daily_minute;

   float pose_x;
   float pose_y;
   float pose_q;

   float lin_vel;
   float ang_vel;

   int wheel_vel_left;
   int wheel_vel_right;
}_rvc_tx_s;



/**
* This struct has instance information of application.
*/
typedef struct{
   _rvc_tx_s tx_data;

   int server_socket;
   int client_socket;

   pthread_t rx_thread;
   pthread_t tx_thread;
   pthread_t moving_thread;

   int tx_run;
   int rx_run;
   int moving_run;

   float dest_x;
   float dest_y;

#ifdef _DEVICE_TEST_
   player_h player;
   camera_h camera;
#endif
}_rvc_instance_s;

/*
 *  Position Variables: g_pos, g_pre_pos, g_cur_pos
 *  Because the position value of RVC resets when mode switches,
 *  we need to record the previous values and update current value,
 *  And lastly global position value.
 */

float g_pos_x, g_pos_y, g_pos_q;      // Global position
float pre_pos_x, pre_pos_y, pre_pos_q;   // Previous position values
float cur_pos_x, cur_pos_y, cur_pos_q;   // Current position values
int g_switch, semaphore = 0;
int stopped = 1;

typedef struct MAP_NODE{

   bool visited_flag;
   bool obstacle;
}Map_Node;
/*
 *  Global Map around RVC
 *  The interval between two point is 20mm.
 *  (The size of map is 10m each for width and height [-50, 50])
 *  if the value is 1, that point is inaccessible. (obstacle exists)
 */
Map_Node g_map[101][101];



float make_theta_in_range(float _theta){
   float theta = _theta;

   while(1){
      if (theta > ESLAB_PI){
         theta -= 2 * ESLAB_PI;
      }
      else if (theta < -ESLAB_PI){
         theta += 2 * ESLAB_PI;
      }
      else{
         break;
      }
   }
   return theta;
}

/*
 *  This function changes the direction of the RVC.
 *  This function gets direction variable 'q' (radian)
 *  and sets RVC's direction to q.
 */
void change_direction(float q){

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
      float delta = q - g_pos_q;
      float sign = q * g_pos_q;

      if(sign > 0){                     // When the sign of two value is same,
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
      else {                           // When the sign of two value is different,
         float q_opposite = q + ESLAB_PI;
         // Make the direction in the range of [-PI, PI]
         q_opposite = make_theta_in_range(q_opposite);
         float new_delta = -(q_opposite - g_pos_q);
         if(new_delta < -range){
            rvc_set_control(RVC_CONTROL_DIR_RIGHT);
         }
         else if(new_delta > range){
            rvc_set_control(RVC_CONTROL_DIR_LEFT);
         }
         else{
            rvc_set_control(RVC_CONTROL_DIR_LEFT);
         }

      }

   }
}

/*
 *  This function moves RVC to the specific spot (X,Y).
 *  (X,Y) is the absolute value in global position coordinate.
 *  This requires setting direction to the goal and
 *  Moving to the goal avoiding the obstacles.
 */

void moving_thread_run(void *instance) {
   _rvc_instance_s *inst = (_rvc_instance_s *)instance;

   int status = 0;
   float x = inst->dest_x;
   float y = inst->dest_y;

   float dir;               // Direction to head to (radian)
   float slope;
   int i = 0;

   while(inst->moving_run){
      i++;
      if(x == g_pos_x){
         if( (y - g_pos_y) > 0){
            dir = ESLAB_PI / 2;
         }
         else{
            dir = -ESLAB_PI / 2;
         }
      }
      else if(y == g_pos_y){
    	  if( (x - g_pos_x) > 0){
    		  dir = 0.0;
    	  }
    	  else{
    		  dir = ESLAB_PI;
    	  }
      }
      else {
         slope = (y - g_pos_y)/(x - g_pos_x);

         // The range is [-PI/2, PI/2]
         dir = atan2(y-g_pos_y, x-g_pos_x);
         dlog_print(DLOG_DEBUG, LOG_TAG, "arctan: %f <- %f", dir, slope);

         // When the range is over [-PI/2, PI/2],
         // which is the range of input to tangent,
         /*
         if((x - g_pos_x) < 0){
            dir = dir + ESLAB_PI;
         }

         */
         // Make the direction in the range of [-PI, PI]
         dir = make_theta_in_range(dir);
      }
     if (i%3 == 0) {
      change_direction(dir);
        rvc_set_control(RVC_CONTROL_DIR_FORWARD);
     }

      // If it meets obstacle,

      // If it reaches the goal point
      float delta_x = x - g_pos_x;
      float delta_y = y - g_pos_y;
      float range = 50.0;
      if( (fabsf(delta_x) < range) && (fabsf(delta_y) < range) ){
         dlog_print(DLOG_DEBUG, LOG_TAG, "Move to function finished");

         break;
      }
     usleep(100000);
   }
   rvc_set_mode(RVC_MODE_SET_PAUSE);
   inst->moving_run = false;
   pthread_join(inst->moving_thread, (void **)&status);
}
void move_to_xy(float x, float y, _rvc_instance_s *instance){
   // Check if move_to_xy function is working
   if(instance->moving_run)
      return;

   instance->dest_x = x;
   instance->dest_y = y;

   instance->moving_run = true;
   if(pthread_create(&instance->moving_thread, NULL, moving_thread_run, (void*)instance) < 0){
      instance->moving_run = false;
      dlog_print(DLOG_DEBUG, LOG_TAG, "moving_thread is failed!");
   }
}


void stop_moveto(_rvc_instance_s *instance){
   rvc_set_mode(RVC_MODE_SET_PAUSE);
   instance->moving_run = false;
   pre_pos_x = 0;
   pre_pos_y = 0;
   pre_pos_q = 0;
   stopped = 1;
}

/*
 *  This function executes the logic to scan the surrounding.
 *  Then, it draws the map about the surrounding.
 */
void initial_scan(){

   // Go right first

}



/*
 *  This function is executed by pthread,
 *  which was created at the app_create stage.
 */
void *t_function(void* data){
   dlog_print(DLOG_DEBUG, LOG_TAG, "Start thread function");

   /*
   move_to_xy(1000.0, 100.0);
   move_to_xy(100.0, 200.0);
   move_to_xy(1200.0, -300.0);
    */

}

void initialize(){
   /*
    *  Initialize the position
    */
   // Initialize global position
   g_pos_x = g_pos_y = g_pos_q = 0.0;
   // Initialize previous position
   pre_pos_x = pre_pos_y = pre_pos_q = 0.0;
   // Initialize current position
   cur_pos_x = cur_pos_y = cur_pos_q = 0.0;

   int i,j;
   for(i=0; i<101; i++){
      for(j=0; j<101; j++){
         g_map[i][j].visited_flag = false;
         g_map[i][j].obstacle = false;
      }
   }

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
 * ************ ************Callback functions *****************************
 */


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
   float range = 0.00001;
   if(pose_x < range && pose_x > -range && pose_y < range && pose_y > -range ){
      if (stopped == 1){
         dlog_print(DLOG_DEBUG, LOG_TAG, "Stopped");
         stopped = 0;
      }
      else {
         float changed_x, changed_y;
         rotate_value_with_q(cur_pos_x, cur_pos_y, pre_pos_q, &changed_x, &changed_y);
         pre_pos_x += changed_x;
         pre_pos_y += changed_y;
         pre_pos_q += cur_pos_q;
         pre_pos_q = make_theta_in_range(pre_pos_q);
         dlog_print(DLOG_DEBUG, LOG_TAG, "Just 0,0");
      }
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
   g_pos_q = make_theta_in_range(g_pos_q);

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

void my_error_cb(rvc_device_error_type_e error, void* data){
   dlog_print(DLOG_DEBUG, LOG_TAG, "error event occurred: %d", error);
}

void my_bumper_cb(unsigned char bumper_left, unsigned char bumper_right, void* data){
   _rvc_instance_s *instance = (_rvc_instance_s *)data;
   if (bumper_left || bumper_right) {
      stop_moveto(instance);

   }
}
/**
* This function registers handler which handles the rvc event.
*/
static void
rvc_register_callback(_rvc_instance_s* instance)
{
   if(instance == NULL){
      return;
   }
   rvc_set_pose_evt_cb(my_pose_changed_cb, NULL);
   rvc_set_mode_evt_cb(my_mode_changed_cb, NULL);
   rvc_set_bumper_evt_cb(my_bumper_cb, instance);
}


/**
* This function is to get the robot information.
*/
static void
get_rvc_info(_rvc_instance_s* instance)
{
   if(instance == NULL){
      return;
   }

   rvc_get_mode((rvc_mode_type_get_e*)&instance->tx_data.mode);
   rvc_get_error((rvc_device_error_type_e*)&instance->tx_data.error);
   rvc_get_wheel_vel((signed short*)&instance->tx_data.wheel_vel_left, (signed short*)&instance->tx_data.wheel_vel_right);
   rvc_get_bumper((unsigned char*)&instance->tx_data.bumper_left, (unsigned char*)&instance->tx_data.bumper_right);
   rvc_get_pose(&instance->tx_data.pose_x, &instance->tx_data.pose_y, &instance->tx_data.pose_q);
   rvc_get_cliff((unsigned char*)&instance->tx_data.cliff_left, (unsigned char*)&instance->tx_data.cliff_center, (unsigned char*)&instance->tx_data.cliff_right);
   rvc_get_lift((unsigned char*)&instance->tx_data.lift_left, (unsigned char*)&instance->tx_data.lift_right);
   rvc_get_magnet((unsigned char*)&instance->tx_data.magnet);
   rvc_get_suction_state((rvc_suction_state_e *)&instance->tx_data.suction);
   rvc_get_reserve(RVC_RESERVE_TYPE_ONCE, (unsigned char*)&instance->tx_data.once_on, (unsigned char*)&instance->tx_data.once_hour, (unsigned char*)&instance->tx_data.once_minute);
   rvc_get_reserve(RVC_RESERVE_TYPE_DAILY, (unsigned char*)&instance->tx_data.daily_on, (unsigned char*)&instance->tx_data.daily_hour, (unsigned char*)&instance->tx_data.daily_minute);
   rvc_get_lin_ang_vel(&instance->tx_data.lin_vel, &instance->tx_data.ang_vel);
   rvc_get_battery_level((rvc_batt_level_e*)&instance->tx_data.battery);
   rvc_get_voice_type((rvc_voice_type_e*)&instance->tx_data.voice);
}
/**
* This function transmits the robot information to a mobile.
*/
static void*
tx_thread_run(void *data)
{
   _rvc_instance_s* instance = (_rvc_instance_s*)data;
    char msg[RVC_JSON_SIZE+1] = {0,};
    struct timeval tv = {0,};
    _rvc_tx_s* tx = &instance->tx_data;

   if(instance == NULL){
      service_app_exit();
      return NULL;
   }

    memset(&tv, 0, sizeof(struct timeval));

    tv.tv_sec = 5;

    setsockopt(instance->client_socket, SOL_SOCKET, SO_SNDTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

   while(instance->tx_run){
      if(instance->client_socket == 0){
         break;
      }

      snprintf(msg, RVC_JSON_SIZE, rvc_pos_object_format, g_pos_x, g_pos_y, g_pos_q);

      if(write(instance->client_socket , (char*)msg , RVC_JSON_SIZE) == -1){
          close(instance->client_socket);
         instance->client_socket = 0;
         break;
      }else{
         dlog_print(DLOG_DEBUG, LOG_TAG, "msg = %s", msg);
         usleep(1000000);
      }
   }

   instance->tx_run = false;
    return NULL;
}

/**
* This function processes a JSON object from the received data.
*/
static void
parse_members(JsonObject* object, const gchar *member_name, JsonNode *member_node, gpointer user_data)
{
   _rvc_instance_s* instance = (_rvc_instance_s*)user_data;

   if(instance == NULL){
      return;
   }

   if (g_strcmp0(member_name, "control") == 0) {
      int ctrl = (int)json_node_get_int(member_node);

      //Move
      if (ctrl == 0) {
         /*JsonObject *obj = json_node_get_object(member_node);
         float x = (float)json_node_get_double(json_object_get_member(obj, "x"));
         float y = (float)json_node_get_double(json_object_get_member(obj, "y"));
         dlog_print(DLOG_DEBUG, LOG_TAG, "[ESEVAN] move %f %f", x, y);
         move_to_xy(x, y, instance);*/
      }
      //Stop
      else if (ctrl == 1) {
         dlog_print(DLOG_DEBUG, LOG_TAG, "[ESEVAN] stop");
         stop_moveto(instance);
      }
   }
   else if(g_strcmp0(member_name, "position") == 0) {
      JsonObject *obj = json_node_get_object(member_node);
      float x = (float)json_node_get_double(json_object_get_member(obj, "x"));
      float y = (float)json_node_get_double(json_object_get_member(obj, "y"));
      dlog_print(DLOG_DEBUG, LOG_TAG, "[ESEVAN] move %f %f", x, y);
      move_to_xy(x, y, instance);
   }
}

/**
* This function processes a JSON object from the received data.
*/
static void
parse_cmd(_rvc_instance_s* instance, char* msg)
{
   JsonParser *jsonParser = NULL;
   GError *error = NULL;

   dlog_print(DLOG_DEBUG, LOG_TAG, "parse_cmd:%s", msg);

   if(instance == NULL){
      return;
   }

   jsonParser = json_parser_new();

   if(jsonParser != NULL){
      if(json_parser_load_from_data(jsonParser, msg, -1, &error)){
         JsonNode *root = NULL;
         JsonObject *object = NULL;

         root = json_parser_get_root(jsonParser);

         if (JSON_NODE_TYPE (root) == JSON_NODE_OBJECT) {
            object = json_node_get_object (root);

            if(object != NULL){
               json_object_foreach_member(object, parse_members, instance);
            }
         }
      }

      if(error != NULL){
         g_error_free (error);
      }

      if(jsonParser != NULL){
         g_object_unref(jsonParser);
      }
   }
}

/**
* This function receives the rvc command from a mobile.
*/
static void*
rx_thread_run(void *data)
{
   _rvc_instance_s* instance = (_rvc_instance_s*)data;
   struct sockaddr_in client_addr = {0,};
   int rx_recv_size = 0;
    int status = 0;
    socklen_t client_addr_size = 0;
    char msg[RVC_JSON_SIZE+1] = {0,};

   if(instance == NULL){
      service_app_exit();
      return NULL;
   }

   while(true){
      if(listen(instance->server_socket, 5) == -1){
         service_app_exit();
         break;
      }

      client_addr_size = sizeof(client_addr);
      instance->client_socket = accept(instance->server_socket, (struct sockaddr*)&client_addr, &client_addr_size);

      if(instance->client_socket==-1){
         service_app_exit();
         break;
      }

      dlog_print(DLOG_DEBUG, LOG_TAG, "[ESEVAN] accepted %d", instance->client_socket);

      instance->tx_run = true;

       if(pthread_create(&instance->tx_thread, NULL, tx_thread_run, (void*)instance) < 0){
          instance->tx_run = false;
             dlog_print(DLOG_DEBUG, LOG_TAG, "tx_thread is failed!");
            service_app_exit();
            break;
        }

       while(instance->tx_run == true){
          memset(msg, 0, RVC_JSON_SIZE);
          rx_recv_size = read(instance->client_socket, msg, RVC_JSON_SIZE);

          if(rx_recv_size > 0){
             dlog_print(DLOG_DEBUG, LOG_TAG, "[ESEVAN] recv msg(%d) : %s",rx_recv_size, msg);
                parse_cmd(instance, msg);
          }else if(rx_recv_size == -1){
             instance->tx_run = false;

             if(instance->client_socket!=0){
                 close(instance->client_socket);
                 instance->client_socket = 0;
             }

             if(instance->tx_thread != 0){
                 pthread_join(instance->tx_thread, (void **)&status);
             }
          }
      }
   }

    return NULL;
}

/**
* This function make a socket for the communication with the Mobile.
*/
static bool
start_server_socket(_rvc_instance_s* instance)
{
   struct sockaddr_in server_addr = {0,};

   if(instance == NULL){
      return false;
   }

   instance->server_socket = socket( PF_INET, SOCK_STREAM, 0);

   if(instance->server_socket == -1){
        dlog_print(DLOG_DEBUG, LOG_TAG, "create failed!");
       return false;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(RVC_SERVER_PORT);
    server_addr.sin_addr.s_addr= htonl(INADDR_ANY);

    if(bind(instance->server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))){
       dlog_print(DLOG_DEBUG, LOG_TAG, "bind failed!");
       return false;
    }

    instance->rx_run = true;

    if(pthread_create(&instance->rx_thread, NULL, rx_thread_run, (void*)instance) < 0){
       instance->rx_run = false;
          dlog_print(DLOG_DEBUG, LOG_TAG, "rx_thread is failed!");
          return false;
      }

    return true;
}


bool service_app_create(void *data)
{
   _rvc_instance_s* instance = (_rvc_instance_s*)data;

   if(instance != NULL){
      if(rvc_initialize()!=RVC_USER_ERROR_NONE){
         dlog_print(DLOG_DEBUG, LOG_TAG, "rvc_initialize is failed!");
         return false;
      }

      if(start_server_socket(instance) == false){
         rvc_deinitialize();
         dlog_print(DLOG_DEBUG, LOG_TAG, "start_server_socket is failed!");
         return false;
      }

      rvc_register_callback(instance);
   }else{
      dlog_print(DLOG_DEBUG, LOG_TAG, "g_instance is null!");
   }
   initialize();

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
   int status = 0;
   _rvc_instance_s* instance = (_rvc_instance_s*)data;

   if(instance!=NULL){
      if(instance->client_socket!=0){
          close(instance->client_socket);
          instance->client_socket = 0;
      }

      if(instance->server_socket!=0){
          close(instance->server_socket);
          instance->server_socket = 0;
      }

      if(instance->rx_thread != 0){
         instance->rx_run = false;
          pthread_join(instance->rx_thread, (void **)&status);
      }

      if(instance->tx_thread != 0){
         instance->tx_run = false;
          pthread_join(instance->tx_thread, (void **)&status);
      }
   }

   dlog_print(DLOG_DEBUG, LOG_TAG, "service_app_terminate");
}

void service_app_control(app_control_h app_control, void *data)
{
    // Todo: add your code here.
}

static void
service_app_lang_changed(app_event_info_h event_info, void *user_data)
{
   /*APP_EVENT_LANGUAGE_CHANGED*/
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
}

static void
service_app_low_memory(app_event_info_h event_info, void *user_data)
{
   /*APP_EVENT_LOW_MEMORY*/
}

int main(int argc, char* argv[])
{
   _rvc_instance_s instance;
   memset(&instance, 0, sizeof(_rvc_instance_s));

   service_app_lifecycle_callback_s event_callback;
   app_event_handler_h handlers[5] = {NULL, };

   event_callback.create = service_app_create;
   event_callback.terminate = service_app_terminate;
   event_callback.app_control = service_app_control;

   service_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, service_app_low_battery, &instance);
   service_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, service_app_low_memory, &instance);
   service_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, service_app_lang_changed, &instance);
   service_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, service_app_region_changed, &instance);

   return service_app_main(argc, argv, &event_callback, &instance);
}
