#Arduino SerialBuster

A protocol for communicating between several nodes on a single serial bus. This protocol is mainly developed for an RS485 network with one master and a number of slave devices.

+   Non blocking, meaning no delays blocking the loop() from running
+   Send arbitrary sized package payloads
+   Binary safe
+   CRC8 checksum

Arduino version of node-serialbuster. Please go here to read more: https://github.com/mgunneras/node-serialbuster