#include <AccelStepper.h>
#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

AccelStepper noseTrim(AccelStepper::FULL4WIRE, 40, 38, 36, 34);
AccelStepper tacho(AccelStepper::FULL4WIRE, A2, A3, A4, A5);
AccelStepper radTemp(AccelStepper::FULL4WIRE, 42, 44, 46, 48);
AccelStepper oilTemp(AccelStepper::FULL4WIRE, 47, 49, 51, 53);
AccelStepper boost(AccelStepper::FULL4WIRE, A6, A7, A8, A9);
AccelStepper volt(AccelStepper::FULL4WIRE, 26, 27, 28, 29);
AccelStepper fuel(AccelStepper::FULL4WIRE, 39, 41, 43, 45);

AccelStepper climb(AccelStepper::FULL4WIRE, 14, 15, 16, 17);
AccelStepper slip(AccelStepper::FULL4WIRE, 4, 5, 6, 7);
AccelStepper turn(AccelStepper::FULL4WIRE, 8, 9, 10, 11);
AccelStepper diStepper(AccelStepper::DRIVER, 50, 52);
AccelStepper asi(AccelStepper::DRIVER, A0, A1);

#define di_IRSensor 12
int DI_encoderPin1 = 2;
int DI_encoderPin2 = 3;

#define asi_IRSensor 13
int asiDelta = 0;
int asiDeltaLast = 0;

#define fuelPin A14

const unsigned long DATA_TIMEOUT = 500; // Adjust as needed
unsigned long lastReceivedTime = 0;

void setup() {
  // put your setup code here, to run once:
  noseTrim.setAcceleration(1000);
  noseTrim.setCurrentPosition(0);
  noseTrim.setMaxSpeed(1000);

  tacho.setAcceleration(1000);
  tacho.setCurrentPosition(0);
  tacho.setMaxSpeed(1000);

  radTemp.setAcceleration(1000);
  radTemp.setCurrentPosition(0);
  radTemp.setMaxSpeed(1000);

  oilTemp.setAcceleration(1000);
  oilTemp.setCurrentPosition(0);
  oilTemp.setMaxSpeed(1000);

  boost.setAcceleration(1000);
  boost.setCurrentPosition(0);
  boost.setMaxSpeed(1000);

  volt.setAcceleration(1000);
  volt.setCurrentPosition(0);
  volt.setMaxSpeed(1000);

  fuel.setAcceleration(5000);
  fuel.setCurrentPosition(0);
  fuel.setMaxSpeed(2000);

  climb.setAcceleration(1000);
  climb.setCurrentPosition(-300);
  climb.setMaxSpeed(1000);

  turn.setAcceleration(1000);
  turn.setCurrentPosition(-122);
  turn.setMaxSpeed(1000);
  
  slip.setAcceleration(1000);
  slip.setCurrentPosition(0);
  slip.setMaxSpeed(1000);

  //DI
  //diStepper.setMaxSpeed(1000);
  //diStepper.setAcceleration(1000);
  //int diIR = digitalRead(di_IRSensor);
  //pinMode(di_IRSensor, INPUT);
  //pinMode(DI_encoderPin1, INPUT); 
  //pinMode(DI_encoderPin2, INPUT);
  //digitalWrite(DI_encoderPin1, HIGH); //turn pullup resistor on
  //digitalWrite(DI_encoderPin2, HIGH); //turn pullup resistor on
  //diStepper.setSpeed(750);
  //while(diIR == 1){
  //  diIR = digitalRead(di_IRSensor);
  //  diStepper.runSpeed();
  //}
  //diStepper.setCurrentPosition(2000);
  //diStepper.moveTo(0);

  //ASI
  //asi.setMaxSpeed(1000);
  //asi.setAcceleration(1000);
  //int asiIR = digitalRead(asi_IRSensor);
  //pinMode(asi_IRSensor, INPUT);
  //asi.setSpeed(750);
  //while(asiIR == 1){
  //  asiIR = digitalRead(asi_IRSensor);
  //  asi.runSpeed();
  //}
  //asi.setCurrentPosition(720);
  //asi.moveTo(0);

  DcsBios::setup(); //DCS BIOS Setup
}


void onTrimgaugeChange(unsigned int newValue) {
    int tempVal = map(newValue, 0, 65535, 68, -68);
    noseTrim.moveTo(tempVal);
}
DcsBios::IntegerBuffer trimgaugeBuffer(0x542e, 0xffff, 0, onTrimgaugeChange);

void onTachometergaugeChange(unsigned int newValue) {
    long inLookup[] = {0, 13107, 0.4*65535, 0.6*65535, 0.8*65535, 65535};
    long outLookup[] = {0, 0, -150, -300, -450, 600};
    float tempVal = interpolate(inLookup, outLookup, newValue);
    tacho.moveTo(tempVal);
}
DcsBios::IntegerBuffer tachometergaugeBuffer(0x544e, 0xffff, 0, onTachometergaugeChange);

void onRadiatortemperaturegaugeChange(unsigned int newValue) {
    long inLookup[] = {0, 19660, 24258, 28753, 31914, 35831, 40429, 43288, 46760, 48787, 51161, 53320, 65535};
    long outLookup[] = {0, 30, 52, 70, 100, 135, 190, 225, 255, 295, 330, 380, 600};
    float tempVal = interpolate(inLookup, outLookup, newValue);
    radTemp.moveTo(tempVal);
}
DcsBios::IntegerBuffer radiatortemperaturegaugeBuffer(0x5456, 0xffff, 0, onRadiatortemperaturegaugeChange);

void onOiltemperaturegaugeChange(unsigned int newValue) {
    int tempVal = map(newValue, 0, 65535, 0, 600);
    oilTemp.moveTo(tempVal);
}
DcsBios::IntegerBuffer oiltemperaturegaugeBuffer(0x5454, 0xffff, 0, onOiltemperaturegaugeChange);

void onBoostgaugeChange(unsigned int newValue) {
    int tempVal = map(newValue, 0, 65535, 0, 600);
    boost.moveTo(tempVal);
}
DcsBios::IntegerBuffer boostgaugeBuffer(0x5450, 0xffff, 0, onBoostgaugeChange);

void onVoltmetergaugeChange(unsigned int newValue) {
    int tempVal = map(newValue, 0, 65535, 75, -75);
    volt.moveTo(tempVal);
}
DcsBios::IntegerBuffer voltmetergaugeBuffer(0x544c, 0xffff, 0, onVoltmetergaugeChange);

//on change callback function
void onVariometergaugeChange(unsigned int newValue) {
  //convert climb indicator
  float incomingByte = map(newValue, 0, 65535, -600, 0);
  climb.moveTo(incomingByte);
}

DcsBios::IntegerBuffer variometergaugeBuffer(0x543c, 0xffff, 0, onVariometergaugeChange);

void onSideslipgaugeChange(unsigned int newValue) {
  long inLookup[] = {0, 13539, 51996, 65535}; //input from 0-65535
  long outLookup[] = {-93, -63, 63, 93}; //output in steps
  float out = interpolate(inLookup, outLookup, newValue);
  slip.moveTo(out);
  }
DcsBios::IntegerBuffer sideslipgaugeBuffer(0x5448, 0xffff, 0, onSideslipgaugeChange);

void onTurngaugeChange(unsigned int newValue) {\
  float incomingByte = map(newValue, 0, 65535, 0, -244);
  turn.moveTo(incomingByte);
}
  
DcsBios::IntegerBuffer turngaugeBuffer(0x544a, 0xffff, 0, onTurngaugeChange);

void onDigaugeChange(unsigned int newValue) {
  int steps = 5554;
  long target = map(newValue, 0, 65535, 0, steps);
  int delta = target - diStepper.currentPosition();
  int move = (delta) % (steps);
  if(move < -steps/2){
    move += steps;
  }
  else if(move > steps/2){
    move -= steps;
  }
  //diStepper.move(move);
}
DcsBios::IntegerBuffer digaugeBuffer(0x5446, 0xffff, 0, onDigaugeChange);

DcsBios::RotaryEncoder di("DI", "-3200", "+3200", DI_encoderPin2, DI_encoderPin1);

float interpolate(long inArr[], long outArr[], long inVal){
  for(int i = 0; i<=100; i++){
    if(inVal == inArr[i]){
      return outArr[i];
    }
    else if(inVal > inArr[i] && inVal < inArr[i + 1]){
      long x1 = inArr[i];
      long y1 = outArr[i];
      long x2 = inArr[i+1];
      long y2 = outArr[i+1];
      float outVal = y1 + ((float)(y2-y1) / (x2-x1)) * (inVal - x1);
      return outVal;
    }
  }
}

DcsBios::Switch2Pos fuelGauge("FUEL_GAUGE", fuelPin);

void onFuelreservegaugeChange(unsigned int newValue) {
  long inLookup[] = {0, 3000, 65535}; //input from 0-65535
  long outLookup[] = {0, 15, 158}; //output in steps
  float out = interpolate(inLookup, outLookup, newValue);
  fuel.moveTo(out);
}
DcsBios::IntegerBuffer fuelreservegaugeBuffer(0x5458, 0xffff, 0, onFuelreservegaugeChange);

void onAirspeedgaugeChange(unsigned int newValue) {
  long inLookup[] = {0, 7821, 9031, 10556, 11806, 13015, 14378, 15618, 16729, 18248, 19763, 21057, 22185, 23686, 24974, 26259, 27647, 28878, 30259, 31579, 32753, 34119, 35485, 36821, 38109, 39342, 40421, 41835, 43331, 44462, 45725, 47103, 48579, 50009, 51328, 52534, 53980, 55307, 56293, 57817, 58944, 60146, 61603, 65535}; //input from 0-65535
  long outLookup[] = {0, -73, -118, -190, -255, -335, -425, -520, -615, -720, -830, -955, -1075, -1220, -1350, -1475, -1615, -1750, -1890, -2030, -2140, -2270, -2385, -2505, -2600, -2710, -2810, -2910, -3010, -3110, -3215, -3315, -3420, -3520, -3625, -3730, -3820, -3925, -4025, -4130, -4230, -4330, -4430, -4530}; //output in steps
  float out = interpolate(inLookup, outLookup, newValue);
  int asiDelta = out - asi.currentPosition();

  int backlashStepsCCW = 25;
  int backlashStepsCW = 10;
  if(asiDelta > 0 && asiDeltaLast < 0){
    out += backlashStepsCCW;
    //Serial.println("ccw, + backlash");
  }
  else if(asiDelta < 0 && asiDeltaLast > 0){
    out -= backlashStepsCW;
    //Serial.println("cw, - backlash");
  }
  asiDeltaLast = asiDelta;
  //asi.moveTo(out);
}
DcsBios::IntegerBuffer airspeedgaugeBuffer(0x5436, 0xffff, 0, onAirspeedgaugeChange);

void loop() {
  // put your main code here, to run repeatedly:
  DcsBios::loop();
  noseTrim.run();
  tacho.run();
  radTemp.run();
  oilTemp.run();
  boost.run();
  volt.run();
  fuel.run();
  slip.run();
  turn.run();
  climb.run();
  diStepper.run();
  asi.run();
  if (Serial.available() > 0) {
        // Update last message timestamp if data is available
      lastReceivedTime = millis();
    }
  if (millis() - lastReceivedTime > DATA_TIMEOUT) {
    noseTrim.moveTo(0);
    tacho.moveTo(0);
    radTemp.moveTo(0);
    oilTemp.moveTo(0);
    boost.moveTo(0);
    volt.moveTo(0);
    fuel.moveTo(0);
    climb.moveTo(-300);
    turn.moveTo(-122);
    slip.moveTo(0);
    int move = (1020 - diStepper.currentPosition()) % (2040);
    if(move < -1020){
      move += 2040;
    }
    else if(move > 1020){
      move -= 2040;
    }
    //diStepper.moveTo(1020);
  }
}
