# IoTLED

The aim of this Project is to create, setup and program an Internet of Things device capable of controlling adressable LED-Strips like WS2813.

The goal is to control 2 LED-Strips over an ESP32-driven webapp, in which users can select between different lightning setups and also a Music-Visualizer mode using an AUX input.

The webapp used to control the LEDs is designed as a single-page-webapp, with everything written in native HTML, JS and CSS.
With the goal to make the webapp as user-friendly as possible, I've opted for a [material design](https://material.io/design/) utilising the W3.CSS framework.

## Built with
* [arduino-esp32](https://github.com/espressif/arduino-esp32) - Arduino Core for ESP32
* [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) - Asynchronous Webserver Library with Websocket integration
* [FastLED](https://github.com/FastLED/FastLED) - Library for controlling LEDs
* [SparkFun Spectrum Shield](https://www.sparkfun.com/products/13116) - Arduino Shield with MSGEQ7 Audio Band ICs
* [W3.CSS Framework](https://www.w3schools.com/w3css/default.asp) - W3school's CSS Framework used to build the website


## Tremendous help from here
* https://randomnerdtutorials.com/esp32-web-server-arduino-ide
* https://tronixstuff.com/2013/01/31/tutorial-arduino-and-the-msgeq7-spectrum-analyzer/
* [Devin Crawfords LED-Visualizer Project](https://github.com/DevonCrawford/LED-Music-Visualizer/blob/master/src/LED_Visualizer.ino)
* [This awesome Video by BRUH Automation](https://www.youtube.com/watch?v=9KI36GTgwuQ)


## Goals

* Finalize Hardware setup to work with external power supply and draw circuit configuration etc.
* ~~Getting LEDs to work with hardware setup~~
* ~~Setup webserver~~
* ~~Configure webserver to control LEDs~~
* ~~Get webserver to transfer data with the controller~~
* ~~Finish webapp/webserver-backend~~
* ~~Finish webapp frontend (designing)~~
* Integrate music visualizer
* Finalize webapp-design and LED effects
* _Integrate Google Assistant (?)_ 
