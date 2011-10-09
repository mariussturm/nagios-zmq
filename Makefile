nagios-zmq.o: nagios-zmq.c

.c.o:
	gcc -shared -fPIC -lzmq -ljson -luuid -o $@ $<

client:
	gcc -I ./include -fPIC -lzmq -o zmq_client zmq_client.c

clean:
	rm -f *.o

install:
	install nagios-zmq.o /var/lib/nagios3/

