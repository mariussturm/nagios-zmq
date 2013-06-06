CC := gcc
CFLAGS :=
LDFLAGS :=

nagios-zmq.o: nagios-zmq.c

.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -fPIC -lzmq -ljson -luuid -o $@ $<

client:
	$(CC) $(CFLAGS) $(LDFLAGS) -I ./include -fPIC -lzmq -o zmq_client zmq_client.c

clean:
	rm -f *.o zmq_client

install:
	install nagios-zmq.o /var/lib/nagios3/

