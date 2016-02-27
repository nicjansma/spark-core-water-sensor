//
// Particle (Spark) Core/Photon/Electron Water Alarm + Temperature/Humidity Logger
// By Nic Jansma
// http://nicj.net
//
// This is a combination of a Water Alarm (http://github.com/nicjansma/spark-water-alarm)
// and a DHT temperature/humidity sensor (http://github.com/nicjansma/dht-logger)
//

//
// Includes
//
#include "application.h"
#include "DHT.h"
#include "Adafruit_IO_Client.h"
#include "Adafruit_IO_Arduino.h"

//
// Configuration
//

// pins
#define WATER_SENSOR D0
#define DHT_SENSOR D3
#define LED D7

// How long to wait before noting that the alarm has switched states -- this
// helps stop a super-sensitive sensor from ping-ponging back and forth a lot.
#define DEBOUNCE_SECONDS 300

// device name
#define DEVICE_NAME "wateralarm"

// sensor type: [DHT11, DHT22, DHT21, AM2301]
#define DHTTYPE DHT22

// whether to use Farenheit instead of Celsius
#define USE_FARENHEIT 1

// min/max values (sanity checks)
#define MIN_TEMPERATURE -30
#define MAX_TEMPERATURE 120

#define MIN_HUMIDITY 0
#define MAX_HUMIDITY 100

// temperature/humidity sensor sending interval (seconds)
#define SEND_INTERVAL 60

// AdaFruit integration
#define ADAFRUIT_API_KEY "27df87804133673ce466a5f90334cf11affeaf2d"
#define ADAFRUIT_FEED_TEMPERATURE "wateralarm-temp"
#define ADAFRUIT_FEED_HUMIDITY "wateralarm-humidity"
#define ADAFRUIT_FEED_HEAT_INDEX "wateralarm-heat-index"

// Particle event
#define PARTICLE_LOG_EVENT_NAME "iot-log"

//
// Vars
//

// whether or not water is sensed: 0 = no, 1 = yes
int water = 0;

// the alarm state: 0 = off, 1 = on
int alarmState = 1;

// the last time we switched alarm states (ms since Unix epoch)
int lastStateSwitchTime = 0;

// TCP client
TCPClient tcpClient;

// DHT sensor object
DHT dht(DHT_SENSOR, DHTTYPE);

// current values
float humidity;
float temperature;
float heatIndex;

char humidityString[10];
char temperatureString[10];
char heatIndexString[10];

// last time since we sent sensor readings
int lastUpdate = 0;

// Adafruit feeds
Adafruit_IO_Client aio = Adafruit_IO_Client(tcpClient, ADAFRUIT_API_KEY);
Adafruit_IO_Feed aioFeedTemperature = aio.getFeed(ADAFRUIT_FEED_TEMPERATURE);
Adafruit_IO_Feed aioFeedHumidity = aio.getFeed(ADAFRUIT_FEED_HUMIDITY);
Adafruit_IO_Feed aioFeedHeatIndex = aio.getFeed(ADAFRUIT_FEED_HEAT_INDEX);

// for Particle.publish payloads
char payload[1024];

//
// Functions
//

// Spark setup
void setup() {
    // initialize our pins I/O
    pinMode(LED, OUTPUT);
    pinMode(WATER_SENSOR, INPUT);

    // tell the world we're online
    Particle.publish("online");

    // publish the water variable
    Particle.variable("water", &water, INT);

    // publish the alarm state variable
    Particle.variable("alarmState", &alarmState, INT);

    // temp/humidity vars
    Particle.variable("temperature", &temperatureString[0], STRING);
    Particle.variable("humidity", &humidityString[0], STRING);
    Particle.variable("heatIndex", &heatIndexString[0], STRING);
    
    // start the DHT and Adafruit logging
    dht.begin();
    aio.begin();

    // startup event
    sprintf(payload,
            "{\"device\":\"%s\",\"state\":\"starting\"}",
            DEVICE_NAME);

    Particle.publish(PARTICLE_LOG_EVENT_NAME, payload, 60, PRIVATE);
}

// Spark loop
void loop() {
    checkWater();
    checkDHT();
}

void checkWater() {
    water = isExposedToWater();

    if (water) {
        digitalWrite(LED, HIGH);

        //
        // Alarm ON
        //
        if (alarmState == 0) {
            // only alarm if we're past the debounce interval
            int now = Time.now();
            if (now - lastStateSwitchTime > DEBOUNCE_SECONDS) {
                alarmState = 1;
                lastStateSwitchTime = now;
                Particle.publish("water_alarm", "on", 60, PRIVATE);

                // startup event
                sprintf(payload,
                        "{\"device\":\"%s\",\"state\":\"on\"}",
                        DEVICE_NAME);

                Particle.publish(PARTICLE_LOG_EVENT_NAME, payload, 60, PRIVATE);
            }
        }
    } else {
        digitalWrite(LED, LOW);

        //
        // Alarm off
        //
        if (alarmState == 1) {
            // only alarm if we're past the debounce interval
            int now = Time.now();
            if (now - lastStateSwitchTime > DEBOUNCE_SECONDS) {
                alarmState = 0;
                lastStateSwitchTime = now;
                Particle.publish("water_alarm", "off", 60, PRIVATE);

                // startup event
                sprintf(payload,
                        "{\"device\":\"%s\",\"state\":\"off\"}",
                        DEVICE_NAME);

                Particle.publish(PARTICLE_LOG_EVENT_NAME, payload, 60, PRIVATE);
            }
        }
    }
}

void checkDHT() {
    int now = Time.now();

    // only run every SEND_INTERVAL seconds
    if (now - lastUpdate < SEND_INTERVAL) {
        return;
    }

    lastUpdate = now;

    // read humidity and temperature values
    humidity = dht.readHumidity();
    temperature = dht.readTemperature(USE_FARENHEIT);

    if (temperature == NAN
        || humidity == NAN
        || temperature > MAX_TEMPERATURE
        || temperature < MIN_TEMPERATURE
        || humidity > MAX_HUMIDITY
        || humidity < MIN_HUMIDITY) {
        // if any sensor failed, bail on updates
        return;
    }

    // calculate the heat index
    heatIndex = dht.computeHeatIndex(temperature, humidity, USE_FARENHEIT);

    // convert floats to strings for Particle variables
    sprintf(temperatureString, "%.2f", temperature);
    sprintf(humidityString, "%.2f", humidity);
    sprintf(heatIndexString, "%.2f", heatIndex);

    aioFeedTemperature.send(temperature);
    aioFeedHumidity.send(humidity);
    aioFeedHeatIndex.send(heatIndex);

    sprintf(payload,
        "{\"device\":\"%s\",\"temperature\":%.2f,\"humidity\":%.2f,\"heatIndex\":%.2f}",
        DEVICE_NAME,
        temperature,
        humidity,
        heatIndex);

    Particle.publish(PARTICLE_LOG_EVENT_NAME, payload, 60, PRIVATE);
}

// determine if we're exposed to water or not
boolean isExposedToWater() {
    if (digitalRead(WATER_SENSOR) == LOW) {
        return true;
    } else {
        return false;
    }
}
