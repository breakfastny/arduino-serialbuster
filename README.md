#Arduino SerialBuster

A protocol for communicating between several nodes on a single serial bus. This 
protocol is mainly developed for an RS485 network with one master and a number 
of slave devices.

+   Non blocking, meaning no delays blocking the loop() from running
+   Send arbitrary sized package payloads
+   Binary safe
+   CRC8 checksum

Notes
--

All data is stored as bytes, unsigned int (16 bit) or unsigned long (32 bit). If
you store data that needs signedness, make sure to cast it when reading it.

16 and 32 bit data structures are stored in Buffer as Little Endian.

Compatible libraries
--

 - node.js  https://github.com/breakfastny/node-serialbuster
