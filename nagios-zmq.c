#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <json/json.h>
#include <pthread.h>
#include <uuid/uuid.h>

#include "include/zhelpers.h"

#include "include/nagios/objects.h"
#include "include/nagios/nagios.h"
#include "include/nagios/nebstructs.h"
#include "include/nagios/broker.h"
#include "include/nagios/nebmodules.h"
#include "include/nagios/nebcallbacks.h"

#define DEFAULT_ZMQ_IN_PORT 5555
#define DEFAULT_ZMQ_OUT_PORT 6666

#define LG_INFO 262144
#define LG_WARN  LOG_INFO
#define LG_ERR   LOG_INFO
#define LG_CRIT  LOG_INFO
#define LG_DEBUG LOG_INFO
#define LG_ALERT LOG_INFO
#define MAX_MESSAGE 1024*1024

void *g_nagios_handle;
void *g_context;
void *g_publisher;
int   g_zmq_in_port = DEFAULT_ZMQ_IN_PORT;
int   g_zmq_out_port = DEFAULT_ZMQ_OUT_PORT;
pthread_t g_zmq_forwarder_thread;
pthread_t g_zmq_publisher_thread;

NEB_API_VERSION(CURRENT_NEB_API_VERSION)

void logger(int priority, const char *loginfo, ...)
{
	char buffer[8192];
	snprintf(buffer, 20, "message queue: ");

	va_list ap;
	va_start(ap, loginfo);
	vsnprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), loginfo, ap);
	va_end(ap);
	write_to_all_logs(buffer, priority);
}

void parse_arguments(const char *args) {
	char arguments[1024];
	char *arg_term;

	// no arguments
	if(!args) return;

	strncpy(arguments, args, 1024);
	arg_term = strtok(arguments, " =");
	while(arg_term != NULL) {
		char *key, *value;

		key      = arg_term;
		arg_term = strtok(NULL, " =");
		value    = arg_term;

		if(!strncmp(key, "inport", 4)) {
			g_zmq_in_port = atoi(value);
		}
		if(!strncmp(key, "outport", 4)) {
			g_zmq_out_port = atoi(value);
		}
		arg_term = strtok(NULL, " =");
	} 
}

void *zmq_publisher_start() {
	logger(LG_INFO, "start zmq publisher.");

	// init socket
	g_publisher = zmq_socket(g_context, ZMQ_PUB);
	zmq_connect(g_publisher, "tcp://localhost:5555");
}

void *zmq_forwarder_start() {
	logger(LG_INFO, "start zmq forwarder.");

	// Socket facing clients
	void *incoming = zmq_socket (g_context, ZMQ_SUB);
	zmq_bind (incoming, "tcp://*:5555");
	zmq_setsockopt(incoming, ZMQ_SUBSCRIBE, "", 0);

	// Socket facing services
	void *outgoing = zmq_socket (g_context, ZMQ_PUB);
	zmq_bind (outgoing, "tcp://*:6666");

	// Start built-in device
	zmq_device (ZMQ_FORWARDER, incoming, outgoing);

	// We never get here…
	zmq_close (incoming);
	zmq_close (outgoing);
	zmq_term (g_context);
}

void start_threads() {
	// init zmq context
	g_context = zmq_init(1);

	// start forwarder thread
	pthread_create(&g_zmq_forwarder_thread, 0, zmq_forwarder_start, (void *)0);
	pthread_detach(g_zmq_forwarder_thread);

	// start publisher thread
	pthread_create(&g_zmq_publisher_thread, 0, zmq_publisher_start, (void *)0);
	pthread_detach(g_zmq_publisher_thread);
}

json_object * json_add_pair(json_object *jobj, char *key, char *value) {
	json_object *jstring = json_object_new_string(value);
	json_object_object_add(jobj, key, jstring);

	return jobj;
}

int send_servicecheck(nebstruct_service_check_data *check_data) {
	time_t ts;
	uuid_t uuid;
	char uuid_string[37];
	char message_buffer[MAX_MESSAGE];
	char cast_buffer[1024];

	ts = time(NULL);

	/* Generate the uuid string. */
	uuid_generate(uuid);
	uuid_unparse(uuid, uuid_string);

	json_object * jevent = json_object_new_object();

	sprintf(cast_buffer, "%s",         uuid_string);
	json_add_pair(jevent, "id",        cast_buffer);
	json_add_pair(jevent, "context",   "SERVICECHECK");
	json_add_pair(jevent, "source",    "NAGIOS");
	sprintf(cast_buffer, "%i",         (int)ts);
	json_add_pair(jevent, "timestamp", cast_buffer);

	json_object * jobj = json_object_new_object();

	sprintf(cast_buffer, "%i",             check_data->current_attempt);
	json_add_pair(jobj, "current_attempt", cast_buffer);
	sprintf(cast_buffer, "%i",             check_data->max_attempts);
	json_add_pair(jobj, "max_attempts",    cast_buffer);
	sprintf(cast_buffer, "%i",             check_data->state_type);
	json_add_pair(jobj, "state_type",      cast_buffer);
	sprintf(cast_buffer, "%i",             check_data->state);
	json_add_pair(jobj, "state",           cast_buffer);
	sprintf(cast_buffer, "%ld",            check_data->timestamp.tv_sec);
	json_add_pair(jobj, "timestamp",       cast_buffer);
	sprintf(cast_buffer, "%f",             check_data->execution_time);
	json_add_pair(jobj, "execution_time",  cast_buffer);
	json_add_pair(jobj, "hostname",        check_data->host_name);
	json_add_pair(jobj, "service",         check_data->service_description);
	json_add_pair(jobj, "output",          check_data->output);

	if(check_data->perf_data)
		json_add_pair(jobj, "performance", check_data->perf_data);

	json_object_object_add(jevent, "payload", jobj);

	logger(LG_INFO, "'%s'\n", json_object_to_json_string(jevent));
	sprintf(message_buffer, "%s", json_object_to_json_string(jevent));

	s_send(g_publisher, message_buffer);
	json_object_put(jevent);
	json_object_put(jobj);

	return 0;
}

int broker_check(int event_type, void *data) {
	if (event_type == NEBCALLBACK_SERVICE_CHECK_DATA) {
		nebstruct_service_check_data *c = (nebstruct_service_check_data *)data;
		if (c->type == NEBTYPE_SERVICECHECK_PROCESSED) {
			send_servicecheck(c);
		}
	}
	else if (event_type == NEBCALLBACK_HOST_CHECK_DATA) {
		nebstruct_host_check_data *c = (nebstruct_host_check_data *)data;
		if (c->type == NEBTYPE_HOSTCHECK_PROCESSED) {
			// do stuff...
		}
	}

	return 0;
}

int broker_state(int event_type __attribute__ ((__unused__)), void *data __attribute__ ((__unused__))) {
	return 0;
}

int broker_process(int event_type __attribute__ ((__unused__)), void *data) {
	struct nebstruct_process_struct *ps = (struct nebstruct_process_struct *)data;
	if (ps->type == NEBTYPE_PROCESS_EVENTLOOPSTART) {
		start_threads();
	}

	return 0;
}

void register_callbacks() {
	neb_register_callback(NEBCALLBACK_STATE_CHANGE_DATA,  g_nagios_handle, 0, broker_state);
	neb_register_callback(NEBCALLBACK_SERVICE_CHECK_DATA, g_nagios_handle, 0, broker_check);
	// used for starting threads
	neb_register_callback(NEBCALLBACK_PROCESS_DATA,       g_nagios_handle, 0, broker_process);
}

void deregister_callbacks() {
	neb_deregister_callback(NEBCALLBACK_STATE_CHANGE_DATA,  broker_state);
	neb_deregister_callback(NEBCALLBACK_SERVICE_CHECK_DATA, broker_check);
	neb_deregister_callback(NEBCALLBACK_PROCESS_DATA,       broker_process);
}

/* this function gets called when the module is loaded by the event broker */
int nebmodule_init(int flags __attribute__ ((__unused__)), char *args, void *handle) {
	g_nagios_handle = handle;

	logger(LG_INFO, "nagios-zmq by Marius Sturm");

	parse_arguments(args);
	register_callbacks();

	logger(LG_INFO, "successfully finished initialization");
}

int nebmodule_deinit(int flags __attribute__ ((__unused__)), int reason __attribute__ ((__unused__))) {
	logger(LG_INFO, "deinitializing");
	deregister_callbacks();

	//deinit zmq
	zmq_close(g_publisher);
	zmq_term(g_context);

	return 0;
}
