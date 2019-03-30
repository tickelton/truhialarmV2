# truhialarmV2

![schmeatic](https://gitlab.com/tickelton/truhialarmV2/blob/master/images/truhialamv2_boxed_small.jpg "Schmatic")

truhialarmV2 is a simple IoT project with the goal of sending email notifications when a sensor changes state.

It currently targets the ESP8266 microcontroller, in particular the ESP-01 board.

The code was written for the Arduino development environment for the ESP8266 and has no other external dependencies other than the ESP8266 package.

The particular use case this project was designed for is to monitor the lid of a chest freezer and remind me to update the list of its contents I have pinned to my fridge.

## Hardware

The project consists of only 3 pieces of hardware:

* the ESP-01 itself
* a CH340 USB to serial adapter
* a magnetic door sensor

![schmeatic](https://gitlab.com/tickelton/truhialarmV2/blob/master/schematics/truhialarmv3_minimal_schem.png "Schmatic")

The CH340 Adapter is not actually necessary but only used as a power supply. Alternatively you can use any other type of 3.3v power supply.

I chose to stick with it for production use as it was the most convenient 3.3v power supply I had at hand and it gives me the opportunity to just plug the finished device into a computer and get diagnosis output through a serial terminal if anything need debugging.

For more elaborate debugging circuits and flashing instructions, please refer to the excellent instructions that are pretty much everywhere on the internet, e.g.:

https://www.allaboutcircuits.com/projects/breadboard-and-program-an-esp-01-circuit-with-the-arduino-ide/
http://www.arjunsk.com/iot/iot-using-esp8266-programmer-ch340g-chip-adapter/


## Software

The software is contained in a single source file to be compiled and uploaded with the Arduino IDE.

Please refer to [the above link](https://www.allaboutcircuits.com/projects/breadboard-and-program-an-esp-01-circuit-with-the-arduino-ide/) for detailed instructions.

## Sensor

The sensor is a cheap magnetic door sensor. Since the sensor just acts as a switch the project works just as well with all other kinds of sensor that behave this way, e.g. simple push button switches for breadboard use.

## Case

The case that can be seen in the image on top was laser cut from 3mm acrylic.

The SVG file to recreate the case can be found in the images directory.

## Annotations

While the ESP-01 board does have two dedicated GPIO pins, in this case the sensor is connected to neither of them but the RX pin is reconfigured to act as GPIO3 and the sensor is attached there.

The reason for this is that on the ESP-1 GPIO0 and GPIO2 are overloaded with the selection of the programming/bootloader mode.

For more details please refer to the excellent guide on the topic that can be found here:
https://www.forward.com.au/pfod/ESP8266/GPIOpins/ESP8266_01_pin_magic.html

## Copyright

truhialamV2 is licensed under the ISC license.

See LICENSE for details.

Copyright (c) 2019 tick <tickelton@gmail.com>
