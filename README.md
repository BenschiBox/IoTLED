# IoTLED

The aim of this Project is to create, setup and program an Internet of Things device / network capable of controlling adressable LED-Strips like WS2813.

For now the goal is to control 2 LED-Strips over an ~~android app~~ ESP32-driven webserver, in which the user can select between different lightning setups and also a Music-Visualizer mode using an AUX input.


## Built with
* [arduino-esp32](https://github.com/espressif/arduino-esp32) - Arduino Core for ESP32
* [FastLED](https://github.com/FastLED/FastLED) - Library for controlling LEDs
* [SparkFun Spectrum Shield](https://www.sparkfun.com/products/13116) - Arduino Shield with MSGEQ7 Audio Band ICs


## Tremendous help from here
* https://randomnerdtutorials.com/esp32-web-server-arduino-ide
* https://tronixstuff.com/2013/01/31/tutorial-arduino-and-the-msgeq7-spectrum-analyzer/
* [Devin Crawfords LED-Visualizer Project](https://github.com/DevonCrawford/LED-Music-Visualizer/blob/master/src/LED_Visualizer.ino)
* [This awesome Video by BRUH Automation](https://www.youtube.com/watch?v=9KI36GTgwuQ)


## Goals

* Finalize Hardware setup to work with external power supply and draw circuit configuration etc.
* ~~Getting LEDs to work with hardware setup~~
* Program custom LED effects
* ~~Setting up webserver~~
* ~~Configure webserver to control LEDs~~
* ~~Get webserver to transfer data with the controller~~
* Integrate music visualizer
* Finalize Website-Design and LED effects
* Integrate (Google) Assistant(s) _(?)_ 
