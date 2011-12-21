# nagios-zmq

`nagios-zmq` is a nagios event broker module that provides nagios information on a zeromq publish/subscribe socket.

This allows you to get the check/notification on a 0mq without polling nagios all the time.

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

## Configuration
Add the broker module to your `nagios.cfg`.

    broker_module=/var/lib/nagios3/nagios-zmq.o

* Subscribe to outgoing messages on port 6666.
* Send incoming messages to port 5555.

## Sample output

    [1324481478] message queue: '{ "id": "794e3a38-00df-4d61-b31a-ddcab2f4be54", "context": "NOTIFICATION", "source": "NAGIOS",
    "timestamp": "1324481478", "payload": { "hostname": "localhost", "start_time": "1324481478", "end_time": "1324481478",
    "service_description": "Swap Usage", "reason_type": "0", "state": "2", "output": "SWAP CRITICAL - 100% free (0 MB out of 0 MB)",
    "escalated": "0", "contacts_notified": "1" } }'

    [1324481478] message queue: '{ "id": "9b630c2d-54e8-4a4b-931a-593cb665fb8f", "context": "SERVICECHECK", "source": "NAGIOS",
    "timestamp": "1324481478", "payload": { "current_attempt": "1", "max_attempts": "1", "state_type": "1", "state": "2",
    "timestamp": "1324481478", "execution_time": "0.011632", "hostname": "localhost", "service": "Swap Usage",
    "output": "SWAP CRITICAL - 100% free (0 MB out of 0 MB)", "performance": "swap=0MB;0;0;0;0" } }'

    [1324481488] message queue: '{ "id": "522799f4-ed65-4a18-90b9-0116b29ddc5e", "context": "HOSTCHECK", "source": "NAGIOS",
    "timestamp": "1324481488", "payload": { "current_attempt": "1", "max_attempts": "10", "state_type": "1", "state": "0",
    "timestamp": "1324481488", "execution_time": "4.025331", "hostname": "localhost",
    "output": "PING OK - Packet loss = 0%, RTA = 0.05 ms",
    "performance": "rta=0.054000ms;3000.000000;5000.000000;0.000000 pl=0%;80;100;0" } }'

## Listening
### Sample C-client
The project provides a C-client listerer 'zmqclient'

    ./zmq_client

### Sample Ruby-client

    $ gem instal json
    $ gem instal zmq

    $ ruby zmq_client.rb
    Received service check:
    {"execution_time"=>"4.014652",
     "current_attempt"=>"1",
     "timestamp"=>"1324484668",
     "service"=>"PING",
     "max_attempts"=>"1",
     "output"=>"PING OK - Packet loss = 0%, RTA = 0.06 ms",
     "performance"=>"rta=0.058000ms;100.000000;500.000000;0.000000 pl=0%;20;60;0",
     "hostname"=>"localhost",
     "state"=>"0",
     "state_type"=>"1"}
    Received host check:
    {"execution_time"=>"4.013960",
     "current_attempt"=>"1",
     "timestamp"=>"1324485029",
     "max_attempts"=>"10",
     "output"=>"PING OK - Packet loss = 0%, RTA = 0.06 ms",
     "performance"=>
      "rta=0.057000ms;3000.000000;5000.000000;0.000000 pl=0%;80;100;0",
     "hostname"=>"localhost",
     "state"=>"0",
     "state_type"=>"1"}
    Received notification:
    {"end_time"=>"1324485139",
     "reason_type"=>"0",
     "output"=>"Connection refused",
     "contacts_notified"=>"1",
     "service_description"=>"HTTP",
     "hostname"=>"localhost",
     "escalated"=>"0",
     "state"=>"2",
     "start_time"=>"1324485139"}
