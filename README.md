# nagios-zmq

`nagios-zmq` is a nagios event broker module that provides nagios information
on a zeromq publish/subscribe socket.

## Installation

	sudo apt-get install libjson0-dev uuid-dev

	install libzmq 2.1 from http://www.zeromq.org

	make
	make install
	make client
	./zmq_client


Add the broker module to your `nagios.cfg`.

	broker_module=/var/lib/nagios3/nagios-zmq.o

* Subscribe to outgoing messages on port 6666.
* Send incoming messages to port 5555.
