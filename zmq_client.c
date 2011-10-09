#include "zhelpers.h"

int main (int argc, char *argv [])
{
	void *context = zmq_init(1);

	// Socket to talk to server
	void *subscriber = zmq_socket(context, ZMQ_SUB);
	zmq_connect(subscriber, "tcp://127.0.0.1:6666");
	zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);

	printf("Collecting updates from nagios server...\n");
	while(1) {
		char *string = s_recv(subscriber);
		printf("message: %s\n", string);
		free(string);
	}

	zmq_close(subscriber);
	zmq_term(context);

	return 0;
}
