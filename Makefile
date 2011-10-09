nagios-zmq.o: nagios-zmq.c

.c.o:
	gcc -shared -fPIC -lzmq -ljson -luuid -o $@ $<

clean:
	rm -f *.o

install:
	install nagios-zmq.o /var/lib/nagios3/

