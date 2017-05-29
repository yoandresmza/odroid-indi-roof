An INDI (http://indilib.org/) driver to control a roll off roof of an astronomical observatory.

![indi-driver](https://raw.githubusercontent.com/dokeeffe/odroid-indi-roof/master/docs/indi-driver.png)

## TODO: UPDATE README

Relays attached to GPIO pins of an Odroid board control linear actuators which move the roof.

in folder: switches-read-test
gcc -o read read.c -lwiringPi -lpthread
This will allow you to test the end limit switches
