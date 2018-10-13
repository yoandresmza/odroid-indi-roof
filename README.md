# odroid-indi-roof

This indi driver is used to control my observatory box.
It is just a very compact box with an automated lid, opened by two linear actuators, 2 micro-switches act as position sensors (open/close).
The all lot is powered by an Odroid-C1 and a standard 8 relays board.


![alt tag](https://0a629ff30d37757250e1-8d8307f63a5046ee0a2d736269de64a5.ssl.cf1.rackcdn.com/obs_closed.jpg)

![alt tag](https://0a629ff30d37757250e1-8d8307f63a5046ee0a2d736269de64a5.ssl.cf1.rackcdn.com/obs_opened.jpg)

![indi-driver](https://raw.githubusercontent.com/dokeeffe/odroid-indi-roof/master/docs/indi-driver.png)


in folder: switches-read-test .
gcc -o read read.c -lwiringPi -lpthread .
This will allow you to test the end limit switches
