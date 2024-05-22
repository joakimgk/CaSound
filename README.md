Silly project to interface with a Casio calculator, over its 2.5" jack serial connection, with a microcontroller.
The old idea was to be able to play sounds through a small speaker or buzzer, from games on the calculator, using the SEND function.
But now that I know how to do it, I am no longer bored in school making games on the calculator :)

However! It's always interesting to figure out how stuff works, so here is a small ESP8266 (Arduino) sketch which interfaces the Casio.
The calculator performs a little handshake while transmitting the actual numerical value, which the ESP has to take part in.
Essentially, the calculator sends some bytes and expects a certain byte in return. To me, it was interesting to learn how to read a "stream" of bytes from serial without using UDR interrupts (as I would on ATMEL chips), and also learn how to "parse" the HEX bytes and the BCD numeric payload.

The handshake protocol is described in detail here: 
https://www.grindheim.net/electronics/casio-comm.pdf
and a neat implementation is found here:
https://github.com/nsg21/Arduino-Casio-Serial-library/blob/master/CasioSerial.cpp

Oh, one more thing: Since I am using the RX/TX pins to communicate with the calculator, I need some other way of logging from the ESP.
I know there are tools for this kind of thing, but I simply post (using GET actually) to a small Perl script on a server I have, which appends to a txt file.
The log can then be viewed by SSH-ing to that server, and doing `watch -n 1 tail -n 30 log.txt`
