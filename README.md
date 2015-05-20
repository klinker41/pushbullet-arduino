# Pushbullet Arduino Client #

This project is directed toward creating a working Pushbullet application on an Arduino Uno. Some potential applications for it would be personal IoT projects: sending yourself a push when the washer or drier is done, when the dishwasher has finished, or when your plants need watered based on the soil moisture... really anything!

### Introduction ###

Pushbullet is an awesome service that basically allows you to send messages between your devices or to other pushbullet users. They have Android, iPhone, OSX, Windows, Linux, and web apps, and probably others that I don't know about. So, why not make my Arduino also be able to see these pushes and make pushes of its own? That is the goal of this project. 

It will probably not implement all of Pushbullet's functionality ever, it is more of a proof of concept that could be improved upon in the future. To implement more, you'll need either more memory on the Arduino you use, or you'd need to remove methods out of the libraries that do not get used and optimize those that do for size.

### Pushbullet API Overview ###

Pushbullet's open API's are available [here](https://docs.pushbullet.com/#http). The problem with their implementation when relating to Arduino is that the Uno does not have the required power to be able to use HTTPS. The SSL encryption proves either much to slow decrypt or doesn't work at all, so it is not a viable option.

That being said, I sacrificed security a little bit by creating a man in the middle type of server to handle all of the heavy lifting and then forwards the results back to the Arduino for display to the user. This is why you see an "api" module included in the project.

### Hardware Implementation ###

Necessary Hardware:

1. [Arduino Uno](http://www.arduino.cc/en/Main/ArduinoBoardUno)
2. [Adafruit CC3000 WiFi Board](https://www.adafruit.com/products/1469)
3. [S2C Display](http://www.adafruit.com/products/931)
4. 2 Push Buttons
5. 2 10k Resistors

<img src="/images/Pushbullet.png" width="500" height="457">

### Software Implementation ###

There are two main aspects to the project, the API that the Arduino hits which then forwards the request to Pushbullet (acting as a proxy server) and returns the results (reasoning behind this explained above, has to do with HTTPS and the Arduino now having the processing power to break the AES encryption with SSL, so we'll just bypass it).

The server was created using Grails, because that is a tool that I know how to use and it easily allowed me to create the endpoints, not much more reasoning than that behind it.

To get the server up and running, from the command line (you'll need Grails installed):

```
cd api
grails run-app
```

This will spin up your server that you can hit. Next, you'll need to know the IP address of the machine that you just created the server on, for example 192.162.1.120:8080. You can access the server you created either by going to localhost or <your-ip-address>:8080. We'll need to use the second from our Arduino, assuming that both devices are on the same WiFi network.

BUT, I've done all of this already and deployed it to my own server that you can hit (don't abuse it please or I'll take it down). The Arduino sketch already has all necessary information in it for this proxy server, you don't need to change it at all.

Next, we'll get the Arduino code set to run on your device. Go into the arduino folder and open the ```pushbullet.ino``` file. At the top, there are three lines defining your network that you're running on:

```c
#define WLAN_SSID		"myNetwork"
#define WLAN_PASS		"myPassword"
#define	WLAN_SECURITY	WLAN_SEC_WPA2
```

Edit these to reflect your home network that the device should be connecting to (remember, this should be the same as the network your server is currently running on). This allows the Arduino to connect to WiFi.

Next, we need to define your pushbullet access token. You can get yours [here](https://www.pushbullet.com/account). Enter it at the following line, again in the ```pushbullet.ino``` file.

```c
#define API_TOKEN		"blahblahblah"
```

Lastly, to get it to compile, you'll need to copy the ```library``` folder to your Arduino libraries folder, for OSX this is ```~/Documents/Arduino/libraries```. This has implementation details for the OLED screen and WiFi module. Full credit to Adafruit for these.

Now, go ahead and upload the app to your Arduino and as long as your server is running, you should see Pushbullet up and running on the device!

### Images ###

<img src="/images/IMG_20150518_155545.jpg" width="500" height="375">

---

## License ##

```
Copyright (C) 2015 Jacob Klinker

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```