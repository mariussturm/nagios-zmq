# nagios-zmq

`nagios-zmq` is a nagios event broker which provides nagios informations
on a zeromq message bus.


## Installation
sudo apt-get install libjson0-dev
install libzmq 2.1 from http://www.zeromq.org

gem install ffi
gem install ffi-rzmq

make
make install

set in your nagios.cfg
broker_module=/var/lib/nagios3/nagios-zmq.o

Outgonig messages are on port 6666 and incoming messages can send to port 5555.


