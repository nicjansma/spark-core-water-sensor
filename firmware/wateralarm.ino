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
#define DEBOUNCE_SECONDS 5

//
// Vars
//

// the alarm state: 0 = off, 1 = on
int alarmState = 0;

// the last tiem we switched alarm states (ms since Unix epoch)
int lastStateSwitchTime = 0;

//
// Functions
//

// Spark setup
void setup()
{
    // initialize our pins I/O
	pins_init();
	
	// tell the world we're online
	Spark.publish("online");
	
	// publish the alarm state variable
	Spark.variable("alarmState", &alarmState, INT);
}

// Spark loop
void loop()
{
	if(isExposedToWater()) {
	    //
	    // Alarm ON
	    //
	    if (alarmState == 0) {
	        // only alarm if we're past the debounce interval
	        int now = Time.now();
	        if (now - lastStateSwitchTime > DEBOUNCE_SECONDS) {
    	        alarmState = 1;
    	        lastStateSwitchTime = now;
	    	    digitalWrite(LED, HIGH);
		        Spark.publish("alarm", "on", 60, PRIVATE);
	        }
	    }
	} else {
	    //
	    // Alarm off
	    //
	    if (alarmState == 1) {
	        // only alarm if we're past the debounce interval
	        int now = Time.now();
	        if (now - lastStateSwitchTime > DEBOUNCE_SECONDS) {
    	        alarmState = 0;    
    	        lastStateSwitchTime = now;
	            digitalWrite(LED, LOW);
	            Spark.publish("alarm", "off", 60, PRIVATE);
	        }
	    }
	}
}

// initialize our pins
void pins_init()
{
    pinMode(LED, OUTPUT);
	pinMode(WATER_SENSOR, INPUT);
}

// determine if we're exposed to water or not
boolean isExposedToWater()
{
	if (digitalRead(WATER_SENSOR) == LOW) {
		return true;
	} else {
	    return false;
	}
}