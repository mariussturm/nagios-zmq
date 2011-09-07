#include "zhelpers.h"

int main (int argc, char *argv [])
{
    void *context = zmq_init (1);

    // Socket to talk to server
    printf ("Collecting updates from nagios server...\n");
    void *subscriber = zmq_socket (context, ZMQ_SUB);
    zmq_connect (subscriber, "tcp://10.0.0.18:5555");

    // Subscribe to SERVICECHECK
    char *filter = "SERVICECHECK ";
    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, filter, strlen (filter));

    while(1) {
        char *string = s_recv (subscriber);
        printf("message: %s\n", string);
        free (string);
    }

    zmq_close (subscriber);
    zmq_term (context);
    return 0;
}

