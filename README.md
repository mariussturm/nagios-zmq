Installation:
sudo apt-get install libjson0-dev
sudo apt-get install libzmq-dev
gem install ffi
gem install ffi-rzmq
gcc -shared -lzmq -ljson -o neb2zmq.o neb2zmq.c
cp neb2zmq.o /var/nagios3/...

