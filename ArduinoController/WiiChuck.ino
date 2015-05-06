
#include <math.h>
#include <SoftwareSerial.h>

#include "Wire.h"
#include "WiiChuck.h"

#define MAXANGLE 90
#define MINANGLE -90

SoftwareSerial bt(10,11);

WiiChuck chuck = WiiChuck();
int angleStart, currentAngle;
int tillerStart = 0;
double angle;

void setup() {
  //nunchuck_init();
  Serial.begin(115200);
  chuck.begin();
  chuck.update();
  //chuck.calibrateJoy();
  bt.begin(38400);
  
}


void loop() {
  delay(20);
  chuck.update(); 


  /*Serial.print(chuck.readRoll());
    Serial.print(", ");  
  Serial.print(chuck.readPitch());
    Serial.print(", ");  

    Serial.print((int)chuck.readAccelX()); 
    Serial.print(", ");  
    Serial.print((int)chuck.readAccelY()); 
    Serial.print(", ");  

    Serial.print((int)chuck.readAccelZ()); 
    Serial.print(", ");  
    Serial.print((int)chuck.readJoyX());
    Serial.print(", ");  
    Serial.print((int)chuck.readJoyY());
    Serial.print(", ");  
    Serial.print((int)chuck.cPressed());
    Serial.print(", ");  
    Serial.print((int)chuck.zPressed());

    Serial.println();*/

  //if ( bt.available() )
  {
    char myBuffer[25];
    memset( myBuffer, 0, sizeof(myBuffer));
    //sprintf( myBuffer, "S %.1d %.1d E\n\r", chuck.readJoyX(), chuck.readJoyY() );
    sprintf( myBuffer, "S %.1d %.1d %.1d %.1d E\n\r", chuck.readJoyX(), chuck.readJoyY(), chuck.cPressed(), chuck.zPressed() );
    Serial.print(myBuffer);
    Serial.println();
    bt.write( myBuffer );
  }
}
