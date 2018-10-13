# odroid-indi-roof

This indi driver is used to control my observatory box.
It is just a very compact box with an automated lid, opened by two linear actuators, 2 micro-switches act as position sensors (open/close).
The all lot is powered by an Odroid-C1 and a standard 8 relays board.

![alt tag](http://0144b4c2e6b6b1590378-8d8307f63a5046ee0a2d736269de64a5.r81.cf1.rackcdn.com/Gonzo_box_close.jpg)

![alt tag](http://0144b4c2e6b6b1590378-8d8307f63a5046ee0a2d736269de64a5.r81.cf1.rackcdn.com/Gonzo_box_open.jpg)

![indi-driver](https://raw.githubusercontent.com/dokeeffe/odroid-indi-roof/master/docs/indi-driver.png)


in folder: switches-read-test .
gcc -o read read.c -lwiringPi -lpthread .
This will allow you to test the end limit switches
