neb2zmq.o: neb2zmq.c

.c.o:
	gcc -shared -fPIC -lzmq -ljson -o $@ $<

clean:
	rm -f *.o

install:
	install neb2zmq.o /var/lib/nagios3/

