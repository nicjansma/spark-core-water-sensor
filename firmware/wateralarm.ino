//
// Water Alarm
// By Nic Jansma
// http://nicj.net
// http://github.com/nicjansma/spark-water-alarm
//

//
// Constants
//

// pins
#define WATER_SENSOR D0
#define LED D7

//
// Config
//

// How long to wait before noting that the alarm has switched states -- this
// helps stop a super-sensitive sensor from ping-ponging back and forth a lot.
#define DEBOUNCE_SECONDS 300

//
// Vars
//

// whether or not water is sensed: 0 = no, 1 = yes
int water = 0;

// the alarm state: 0 = off, 1 = on
int alarmState = 1;

// the last time we switched alarm states (ms since Unix epoch)
int lastStateSwitchTime = 0;

//
// Functions
//

// Particle setup
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
}

// Spark loop
void loop() {
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
            }
        }
    }
}

// determine if we're exposed to water or not
boolean isExposedToWater() {
    if (digitalRead(WATER_SENSOR) == LOW) {
        return true;
    } else {
        return false;
    }
}
