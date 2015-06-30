# bt-server
A server that essentially pipes bluetooth connections to TCP/IP

There are a few parts to this, but everything is connected by Unix domain sockets.
btd is connected to the bluetooth device and dispatches itself using fork(2) to handle every connection.

uds-tcpd is a uds client and a TCP/IP server which also forks for every connection.

bt-client is a uds client which uses text.

Using these together, one can effectively pipe them together and communicate with a remote bluetooth device over the Internet.

Use this on the Internet at your own risk.
