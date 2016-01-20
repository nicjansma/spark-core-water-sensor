# Spark Core / Photon / Electron Water Sensor

By [Nic Jansma](http://nicj.net)

This is an internet-connected water sensor that will alert you via SMS (or any other webhook via [IFTTT](https://ifttt.com)) if water is sensed.

I am currently using this near my sump-pump to alert me if the pump fails.  It can be run from either AC or battery power. 

## Hardware

For hardware, I'm utilizing a [Spark Core](https://www.spark.io) hooked up to a [Grove Water Sensor](http://www.seeedstudio.com/wiki/Grove_-_Water_Sensor).

Nowadays, there are even cheaper options from Particle (formerly Spark), such as the [Photon](https://store.particle.io) or [Electron](https://store.particle.io).

The Grove Water Sensor's three wires (power, ground and data) are soldered to the top of the device.

![Spark Core](https://github.com/nicjansma/spark-core-water-sensor/raw/master/images/core.jpg "Spark Core")  

I've 3D printed a simple "seat" for the Core to sit on so the external pins don't short.  The STL is included under `case/`.

![Spark Core Case](https://github.com/nicjansma/spark-core-water-sensor/raw/master/images/case.jpg "Spark Core Case")  

The Core is taped to my sump pump's pipe, and the Water Sensor is taped just below ground-level in the sump pump water pit.

![Grove Water Sensor](https://github.com/nicjansma/spark-core-water-sensor/raw/master/images/sensor.jpg "Grove Water Sensor")  

## Firmware

The Spark Core firmware is available under `firmware/`.  It should work for a Photon or Electron as well.

The firmware publishes these two events to the [Spark Cloud](http://docs.spark.io/api/):

* `online` when the device first starts up
* `water_alarm` when water is sensed.  The payload is either `on` or `off`.

The firmware also exposes two variables:

* `alarmStatus`: `0` means not in alarm state, and `1` means the alarm is active.
* `water`: `0` means no water detected, and `1` means water detected.

## Software

The [previous version](https://github.com/nicjansma/spark-core-water-sensor/releases/tag/v1.0.0) of this sensor used a NodeJS app to monitor for events from the Spark Core.  This app would then use the [Twilio](http://twilio.com/) API to send me a SMS when there was an alarm.

Particle now offers [Webhooks](https://docs.particle.io/guide/tools-and-features/webhooks/), which does this job for you.  You don't have to run a NodeJS app.  Once it senses an event from your Spark Core / Photon / Electron, it will publish an event to any web URL you specify.

Instead of using Twilio for SMS, I am now using [IFTTT](https://ifttt.com) to send me an email, SMS and a [Pushbullet](http://pushbullet.com) notification all at once -- for free!

The creation of the Particle Webhook is pretty simple. First, [install](https://docs.particle.io/guide/tools-and-features/cli/) the `particle-cli` NPM module:

```
npm install -g particle-cli
```

Then, make sure you're logged in:
```
particle login
```

Next, create an [IFTTT](https://ifttt.com) account.  Then, create a [IFTTT Maker](https://ifttt.com/maker) page.  On there, look for your private key (under _Your key is_).

Then, create a file called `ifttt.json` into your directory (see the sample in this repo).  Replace `[your maker key]` in that file with your private key.  It should look something like this:
```
{
    "eventName" : "water_alarm",
    "url" : "https://maker.ifttt.com/trigger/alert/with/key/[your maker key]",
    "requestType" : "POST",
    "query" : {
        "value1" : "Water Alarm",
        "value2" : "{{SPARK_EVENT_VALUE}}"
    },
    "mydevices" : true
}
```

Last, you need to tell Particle how to integrate with IFTTT.  Do this:
```
particle webhook create ifttt.json
```

You should see a success message.  You're all set!

## Parts and Cost

Total cost is around $21.90:
* $19 for the [Photon](https://store.particle.io)
* $2.90 for the [Grove Water Sensor](http://www.seeedstudio.com/wiki/Grove_-_Water_Sensor)
* Cables to connect the Water Sensor to the Core
