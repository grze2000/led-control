# Led control
## Led controller

WS2812B led strip controller built on a Atmega328p microcontroller

### Parts
- Atmega328p DIP
- 2x capacitor 22pf
- HM10 or CC41a Bluetooth LE module
- 2x 10kΩ resistor
- 16 MHz crystal

### Circuit diagram

TO DO

## Arduino code

It uses the [SoftwareSerial](https://www.arduino.cc/en/Reference/softwareSerial) library for Bluetooth communication and [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) library to control the led strip

## Led control web app

 It uses [Bluetooth Web API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Bluetooth_API) to communicate with controller
 
 ![](http://grzegorzbabiarz.com/img/ledControl.jpg)
 
 ## Features
 - Turn on/off, change color of selected part of led strip
 - Halloween mode
