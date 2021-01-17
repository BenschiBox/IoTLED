# IoTLED

The aim of this Project is to create, setup and program an Internet of Things device capable of controlling adressable LED-Strips.
This is achieved over an ESP32-driven webapp, in which users can select between different lighting setups and also a Music-Visualizer mode using an AUX input.

The webapp used to control the LEDs is made with Flutter/Dart, which makes the app very user-friendly.

In my particular setup i use 2x WS2813 LED-strips, although it should work with any amount or any FastLED supported chipset, given some tweaking.

## Built with
* [arduino-esp32](https://github.com/espressif/arduino-esp32) - Arduino Core for ESP32
* [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) - Asynchronous Webserver Library with Websocket integration
* [FastLED](https://github.com/FastLED/FastLED) - Library for controlling LEDs
* [SparkFun Spectrum Shield](https://www.sparkfun.com/products/13116) - Arduino Shield with MSGEQ7 Audio Band ICs
* [Flutter](https://flutter.dev/) - Google's Toolkit for building natively compiled applications for mobile, web, and desktop from a single codebase
* [iro.js color picker](https://iro.js.org/) - An HSV color picker widget for JavaScript, with a modern SVG-based user interface

## Tremendous help from here
* https://randomnerdtutorials.com/esp32-web-server-arduino-ide
* https://tronixstuff.com/2013/01/31/tutorial-arduino-and-the-msgeq7-spectrum-analyzer/
* [Devin Crawfords LED-Visualizer Project](https://github.com/DevonCrawford/LED-Music-Visualizer/blob/master/src/LED_Visualizer.ino)
* [This awesome Video by BRUH Automation](https://www.youtube.com/watch?v=9KI36GTgwuQ)


## Goals
* ~~Getting LEDs to work with basic hardware setup~~
* ~~Setup webserver~~
* ~~Configure webserver to control LEDs~~
* ~~Get users to transfer data with the controller~~
* ~~1st webapp/webserver-backend iteration~~
* ~~Design basic webapp frontend~~
* Rewrite the entire webapp from scratch with flutter because fuck you thats why
* Integrate music visualizer
* Finalize webapp-design and LED effects
* Finalize Hardware setup, draw Circuit Diagram for installation of the Strips
* _Integrate Google Assistant (?)_
