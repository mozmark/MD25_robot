/****************************************************************
* Bluetooth control of an MD25. Adapted from James Henderson's  *
* MD25 example by Mark Goodwin.                                 *
*                                                               *
* Original source:                                              *
* http://www.robot-electronics.co.uk/files/arduino_md25_i2c.ino *
****************************************************************/

#include <SoftwareSerial.h>
#include <Wire.h>

#define CMD                 0x10
                                                              // This is a but with arduino 1
#define MD25ADDRESS         0x58                              // Address of the MD25
#define SOFTWAREREG         0x0D                              // Byte to read the software version, Values of 0 eing sent using write have to be cast as a byte to stop them being misinterperted as NULL
#define SPEED1              (byte)0x00                        // Byte to send speed to first motor
#define SPEED2              0x01                              // Byte to send speed to second motor

// Setup some pins for bluetooth tx / rx
int btTX = 2;
int btRX = 3;

String inputString = "";
boolean stringComplete = false;
SoftwareSerial bt(btTX, btRX);

void setup(){
  delay(2000);
  Wire.begin();
  delay(100);
  encodeReset();
  Serial.begin(9600);
  inputString.reserve(400);
}

int x = 0;

void loop(){
  while(Serial.available())
  {
    char inChar = (char)Serial.read();
    Serial.print(inChar);
    if('\r' != inChar)
    {
      inputString += inChar;
    }
    else
    {
      String complete = String(inputString);
      stringComplete = true;
      processCommand(complete);
      inputString = "";
    }
  }
}

void encodeReset(){                                        // This function resets the encoder values to 0
  Wire.beginTransmission(MD25ADDRESS);
  Wire.write(CMD);
  Wire.write(0x20);                                         // Putting the value 0x20 to reset encoders
  Wire.endTransmission();
}

void stopMotor(){                                           // Function to stop motors
  Wire.beginTransmission(MD25ADDRESS);
  Wire.write(SPEED2);
  Wire.write(128);
  Wire.endTransmission();

  Wire.beginTransmission(MD25ADDRESS);
  Wire.write(SPEED1);
  Wire.write(128);
  Wire.endTransmission();
}

void serialEvent() {
  while(bt.available())
  {
    char inChar = (char)bt.read();
    inputString += inChar;
    if('\n' == inChar)
    {
      String complete = String(inputString);
      stringComplete = true;
      inputString = "";
    }
  }
}

void processCommand(String command){
  String target = "";
  String data = "";
  int sep = command.indexOf(":");
  if(-1 != sep) {
    target = command.substring(0, sep);
    data = command.substring(sep + 1);
    if (target == "a"){
      driveMotor(SPEED1, data.toInt());
    }
    if (target == "b"){
      driveMotor(SPEED2, data.toInt());
    }
  }
}

void driveMotor(int motor, int val){
  Wire.beginTransmission(MD25ADDRESS);
  Wire.write(motor);
  Wire.write(val);
  Wire.endTransmission();
}
