# ConnectedSwitch

This project is a quickStart to create a remote wifi switch along with its web interface to control every lampe / cofee machine or anything you want.

## Functionlities

* Remote On / Off from any computer or smartphone
* Remote timer On / Off
* Physical On / Off synchronized with the web app
* Physical timer On / Off synchronized with the web app

## Material

* Raspberry pi :  used as a custom domotic box hosting the web interface
* Sonoff basic wifi switch :  https://sonoff.itead.cc/en/products/sonoff/sonoff-basic
* 3.3V FTDI module : to flash the sonoff wifi switch with your custom firmware

Lest's get started !

## Reprogram Sonoff

The Sonoff switch was made to be reprogrammed manually. It is based on the ESP boards and can be flashed with the Arduino IDE. There is lots of tutorials explaning you how to do it so I won't explain it again here.
I personnaly followed this one which was very lear to me : https://randomnerdtutorials.com/reprogram-sonoff-smart-switch-with-web-server/
Be sure to install ESP boards into the Arduino IDE. Here is a good tutorial : https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/

Once you are able to falsh new firmware, just flash the wifiSwitch.ino

This scripts will connect your sonoff switch to your wifi, then to the futur MQTT broker into your Raspberry and wait for remote orders. It will also listen to the button present on the Sonoff. In Each case, it will trigger the the relay to open or close the circuit.

## Setup your Raspberry

Your Raspberry will play the role of a domotic box. Connect to your local wifi, the same as the one of your wifi switch.


### mosquitto MQTT broker

First, you will need to install mosquitto MQTT broker
```
$ sudo apt install -y mosquitto mosquitto-clients
```
For it to start on boot :
```
$ sudo systemctl enable mosquitto.service
```
To quickly test your mosquitto MQTT broker, open tow terminal on the Rapsberry and execute each of the following command into a different terminal:
```
$ mosquitto_sub -d -t testTopic
$ mosquitto_pub -d -t testTopic -m "Hello world!"
```

You should see "Hello world!" ont the mosquitto_sub terminal


### Node-red

In order to implement your web app to control your switch, you will need Node and Node-red.

first install npm and node:
```
$ sudo apt-get update
$ sudo apt-get upgrade
$ sudo apt-get install nodejs npm
```

Then install node-red:
```
$ sudo npm install -g --unsafe-perm node-red
```

Then start Node-red :
```
$ node-red
```

The node-red interface will be accessible on the port 8080 of your raspberry. Browse this : ```
```
http://<Raspberry_IP>:1880/
```
You will be redirected to the node-red web interface where you will be able to create and code your web app controlling the wifi switch.

Install the following package through the top right menu/manage palette/Install section :
 - node-red-dashboard 

Then in the top right menu, go to import/clipboard and copy paste the **lampeControlInterface.json**

Finally Deploy the Flow cliking on **Deploy** on the top right.

You should be able to access your web app ont the fllowing url :
```
http://<Raspberry_IP>:1880/ui
```

I strongly recommand to secure your wab app at least with a login password (https://nodered.org/docs/security)


### Remote access

In order to access your web app outside of your local wifi, you will need to redirect one of your wifi box port to the <raspberry_IP>:1880. It should be posible to the config interface of your box.
Make sure to make your Rapsberry Ip static.
In my case, I rederected the port 1880 of my wifi box the my <rapsberry_IP>:1880

Great !! Now you can access your web app through your wifi bow ip !
try to access it with your smartphone with this url:
```
http://<wifi_box_IP>:1880/ui
```



If Everything is ok, you should be able to turno ON and OFF the wifi switch with the web app hosted in your Raspberry and accessible anywhere !!
