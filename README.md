# Spark Core Water Sensor

By [Nic Jansma](http://nicj.net)

This is an internet-connected water sensor that will alert you via SMS if water is sensed.

I am currently using this near my sump-pump to alert me if the pump fails.  It can be run from either AC or battery power. 

## Hardware

For hardware, I'm utilizing a [Spark Core](https://www.spark.io) hooked up to a [Grove Water Sensor](http://www.seeedstudio.com/wiki/Grove_-_Water_Sensor).

The Grove Water Sensor's three wires (power, ground and data) are soldered to the top of the device.

![Spark Core](https://github.com/nicjansma/spark-core-water-sensor/raw/master/images/core.jpg "Spark Core")  

I've 3D printed a simple "seat" for the Core to sit on so the external pins don't short.  The STL is included under `case/`.

![Spark Core Case](https://github.com/nicjansma/spark-core-water-sensor/raw/master/images/case.jpg "Spark Core Case")  

The Core is taped to my sump pump's pipe, and the Water Sensor is taped just below ground-level in the sump pump water pit.

![Grove Water Sensor](https://github.com/nicjansma/spark-core-water-sensor/raw/master/images/sensor.jpg "Grove Water Sensor")  

## Firmware

The Spark Core firmware is available under `firmware/`.  

The firmware publishes these two events to the [Spark Cloud](http://docs.spark.io/api/):

* `online` when the device first starts up
* `alarm` when water is sensed.  The payload is either `on` or `off`.

The firmware also exposes a variable `alarmStatus`: `0` means no alarm, and `1` means the alarm is active.

## Software

Something needs to monitor the events the Core is publishing, and to send SMS messages when water is sensed.  

The simple Node.js app under `monitor/` does this.  It will send messages via [Twilio](http://twilio.com/) when the device comes online, and when the device senses water.

You can run the `monitor` app on a home machine, in the cloud, or hosted on a cheap VPS.  Simple copy the `config.json.sample` to `config.json` and update the file for your personal Spark and Twilio tokens and IDs and the phone numbers you want SMS alerts to be sent to.

## Parts and Cost
Total cost is around $42:
* $39 for the [Spark Core](https://www.spark.io) (from [Seeed Studio](http://www.seeedstudio.com) or [MakerShed](http://www.makershed.com))
* $2.90 for the [Grove Water Sensor](http://www.seeedstudio.com/wiki/Grove_-_Water_Sensor)
* Cables to connect the Water Sensor to the Core

In addition, you will need to create a [Twilio](http://twilio.com/) account so you can get SMS alerts.  I pay about $1 a month.