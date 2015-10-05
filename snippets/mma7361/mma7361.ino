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
}
