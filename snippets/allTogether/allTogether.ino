#include <AcceleroMMA7361.h>

#define sleepPin 13
#define selfTest 12
#define zeroG 11
#define senseSelectPin 10

#define xPin A0
#define yPin A1
#define zPin A2

AcceleroMMA7361 accelero;
int x;
int y;
int z;

#include <Wire.h>
#include <HMC5883L_Simple.h>


#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

// Create a compass
HMC5883L_Simple Compass;;

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

void displaySensorDetails(void){
    sensor_t sensor;
    bmp.getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.print  ("Sensor:       "); Serial.println(sensor.name);
    Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
    Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
    Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" hPa");
    Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" hPa");
    Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" hPa");  
    Serial.println("------------------------------------");
    Serial.println("");
    delay(500);
}


void setup(){
    Serial.begin(9600);
    
    pinMode(sleepPin, OUTPUT);
    digitalWrite(sleepPin, HIGH);

    pinMode(zeroG, INPUT);

    pinMode(senseSelectPin, OUTPUT);
    digitalWrite(senseSelectPin, LOW);
    
    //accelero.begin( sleep, selfTest, zeroG, gSelect, xPin, yPin, zPin); 
    accelero.begin(sleepPin, selfTest, zeroG, senseSelectPin, xPin, yPin, zPin);
    //accelero.setARefVoltage(3.3);                   //sets the AREF voltage to 3.3V =>>>> foi ligado em 5V
    accelero.setSensitivity(HIGH);                   //sets the sensitivity to +/-6G
    accelero.calibrate();

    // Magnetic Declination is the correction applied according to your present location
    // in order to get True North from Magnetic North, it varies from place to place.
    // 
    // The declination for your area can be obtained from http://www.magnetic-declination.com/
    // Take the "Magnetic Declination" line that it gives you in the information, 
    //
    // Examples:
    //   Christchurch, 23° 35' EAST
    //   Wellington  , 22° 14' EAST
    //   Dunedin     , 25° 8'  EAST
    //   Auckland    , 19° 30' EAST
    //    
    Compass.SetDeclination(23, 35, 'E');  

    // The device can operate in SINGLE (default) or CONTINUOUS mode
    //   SINGLE simply means that it takes a reading when you request one
    //   CONTINUOUS means that it is always taking readings
    // for most purposes, SINGLE is what you want.
    Compass.SetSamplingMode(COMPASS_SINGLE);

    // The scale can be adjusted to one of several levels, you can probably leave it at the default.
    // Essentially this controls how sensitive the device is.
    //   Options are 088, 130 (default), 190, 250, 400, 470, 560, 810
    // Specify the option as COMPASS_SCALE_xxx
    // Lower values are more sensitive, higher values are less sensitive.
    // The default is probably just fine, it works for me.  If it seems very noisy
    // (jumping around), incrase the scale to a higher one.
    Compass.SetScale(COMPASS_SCALE_130);

    // The compass has 3 axes, but two of them must be close to parallel to the earth's surface to read it, 
    // (we do not compensate for tilt, that's a complicated thing) - just like a real compass has a floating 
    // needle you can imagine the digital compass does too.
    //
    // To allow you to mount the compass in different ways you can specify the orientation:
    //   COMPASS_HORIZONTAL_X_NORTH (default), the compass is oriented horizontally, top-side up. when pointing North the X silkscreen arrow will point North
    //   COMPASS_HORIZONTAL_Y_NORTH, top-side up, Y is the needle,when pointing North the Y silkscreen arrow will point North
    //   COMPASS_VERTICAL_X_EAST,    vertically mounted (tall) looking at the top side, when facing North the X silkscreen arrow will point East
    //   COMPASS_VERTICAL_Y_WEST,    vertically mounted (wide) looking at the top side, when facing North the Y silkscreen arrow will point West  
    Compass.SetOrientation(COMPASS_HORIZONTAL_X_NORTH);

    
    Serial.println("Pressure Sensor Test"); Serial.println("");

    /* Initialise the sensor */
    if(!bmp.begin())
    {
        /* There was a problem detecting the BMP085 ... check your connections */
        Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
        while(1);
    }

    /* Display some basic information on this sensor */
    displaySensorDetails();
}

void loop(){
    x = accelero.getXRaw();
    y = accelero.getYRaw();
    z = accelero.getZRaw();
    Serial.print("x: ");
    Serial.print(x);
    Serial.print("\ty: ");
    Serial.print(y);
    Serial.print("\tz: ");
    Serial.println(z);
    delay(100);                                     //(make it readable)
    float heading = Compass.GetHeadingDegrees();

    Serial.print("Heading: \t");
    Serial.println( heading );   

    /* Get a new sensor event */ 
    sensors_event_t event;
    bmp.getEvent(&event);

    /* Display the results (barometric pressure is measure in hPa) */
    if (event.pressure)
    {
        /* Display atmospheric pressue in hPa */
        Serial.print("Pressure:    ");
        Serial.print(event.pressure);
        Serial.println(" hPa");

        /* Calculating altitude with reasonable accuracy requires pressure    *
         * sea level pressure for your position at the moment the data is     *
         * converted, as well as the ambient temperature in degress           *
         * celcius.  If you don't have these values, a 'generic' value of     *
         * 1013.25 hPa can be used (defined as SENSORS_PRESSURE_SEALEVELHPA   *
         * in sensors.h), but this isn't ideal and will give variable         *
         * results from one day to the next.                                  *
         *                                                                    *
         * You can usually find the current SLP value by looking at weather   *
         * websites or from environmental information centers near any major  *
         * airport.                                                           *
         *                                                                    *
         * For example, for Paris, France you can check the current mean      *
         * pressure and sea level at: http://bit.ly/16Au8ol                   */

        /* First we get the current temperature from the BMP085 */
        float temperature;
        bmp.getTemperature(&temperature);
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" C");

        /* Then convert the atmospheric pressure, and SLP to altitude         */
        /* Update this next line with the current SLP for better results      */
        float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
        Serial.print("Altitude:    "); 
        Serial.print(bmp.pressureToAltitude(seaLevelPressure,
                    event.pressure)); 
        Serial.println(" m");
        Serial.println("");
    }
    else
    {
        Serial.println("Sensor error");
    }

    delay(800);
}
