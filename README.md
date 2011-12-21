# nagios-zmq

`nagios-zmq` is a nagios event broker module that provides nagios information
on a zeromq publish/subscribe socket.

## Installation

### Ubuntu/Debian based
	sudo apt-get install libjson0-dev uuid-dev
	install libzmq 2.1 from http://www.zeromq.org
### Centos 
	sudo yum install json-c-devel zeromq-devel libuuid-devel

## Compilation
	make
	make install
	make client
	./zmq_client


Add the broker module to your `nagios.cfg`.

	broker_module=/var/lib/nagios3/nagios-zmq.o

* Subscribe to outgoing messages on port 6666.
* Send incoming messages to port 5555.
