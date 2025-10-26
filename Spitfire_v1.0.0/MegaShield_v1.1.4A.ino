#include <AccelStepper.h>
#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

//configure accelstepper library for all of the x27.168 gauges
AccelStepper volt(AccelStepper::DRIVER, 9, 8);
AccelStepper tacho(AccelStepper::DRIVER, 6, 7);
AccelStepper boost(AccelStepper::DRIVER, 11, 10);
AccelStepper radTemp(AccelStepper::DRIVER, 13, 12);
AccelStepper oilTemp(AccelStepper::DRIVER, 14, 15);
AccelStepper fuel(AccelStepper::DRIVER, 4, 5);
AccelStepper nose(AccelStepper::DRIVER, 2, 3);
AccelStepper turn(AccelStepper::DRIVER, 22, 23);
AccelStepper slip(AccelStepper::DRIVER, 24, 25);
AccelStepper climb(AccelStepper::DRIVER, 19, 18);

//configure accelstepper for the 15BY stepper motors
AccelStepper diStepper(AccelStepper::DRIVER, A8, A9);
AccelStepper asi(AccelStepper::DRIVER, A12, A13);

#define di_IRSensor 26
int DI_encoderPin1 = A5;
int DI_encoderPin2 = A4;

#define asi_IRSensor 28
int asiDelta = 0;
int asiDeltaLast = 0;

//define pins for various switches and lamps
#define fuelPin 31
#define gearDown A0
#define gearUp A1
#define supercharger 30
#define superchargerLED 32
#define fuelLED 33
#define gunsightSwitchPin 27
#define navigationSwitch 29
#define magneto1Pin 35
#define magneto2Pin 37

void setup() {
  DcsBios::setup(); //DCS BIOS Setup
  //set output and input pins
  pinMode(fuelPin, INPUT);
  pinMode(gearDown, OUTPUT);
  pinMode(gearUp, OUTPUT);
  //Somes switches require the use of a pullup resistor, this configures those
  pinMode(gunsightSwitchPin, INPUT_PULLUP);
  pinMode(navigationSwitch, INPUT_PULLUP);
  pinMode(magneto1Pin, INPUT_PULLUP);
  pinMode(magneto2Pin, INPUT_PULLUP);

  //X27.168 Powered gauge configuration
  //the move command is to send all gauges back to the initial position
  volt.setAcceleration(10000);
  volt.setMaxSpeed(10000);
  volt.move(-1300);

  tacho.setAcceleration(10000);
  tacho.setMaxSpeed(10000);
  tacho.move(-4500);

  boost.setAcceleration(10000);
  boost.setMaxSpeed(10000);
  boost.move(-4500);

  radTemp.setAcceleration(10000);
  radTemp.setMaxSpeed(10000);
  radTemp.move(4500);

  oilTemp.setAcceleration(10000);
  oilTemp.setMaxSpeed(10000);
  oilTemp.move(4500);

  fuel.setAcceleration(10000);
  fuel.setMaxSpeed(10000);
  fuel.move(-1300);

  nose.setAcceleration(10000);
  nose.setMaxSpeed(10000);
  nose.move(1200);

  turn.setAcceleration(10000);
  turn.setMaxSpeed(10000);
  turn.move(-2000);

  slip.setAcceleration(10000);
  slip.setMaxSpeed(10000);
  slip.move(-1500);

  climb.setAcceleration(10000);
  climb.setMaxSpeed(10000);
  climb.move(-4500);

  //zero all X27.168 powered motors
  while(climb.run() == 1){
    volt.run();
    tacho.run();
    boost.run();
    radTemp.run();
    oilTemp.run();
    fuel.run();
    nose.run();
    turn.run();
    slip.run();
    climb.run();
  }
  //set all X27.168 powered motors to a zero position 
  volt.setCurrentPosition(0);
  tacho.setCurrentPosition(0);
  boost.setCurrentPosition(0);
  radTemp.setCurrentPosition(0);
  oilTemp.setCurrentPosition(0);
  fuel.setCurrentPosition(0);
  nose.setCurrentPosition(0);
  turn.setCurrentPosition(0);
  slip.setCurrentPosition(0);
  climb.setCurrentPosition(0);

  //DI
  diStepper.setMaxSpeed(1000);
  diStepper.setAcceleration(1000);
  int diIR = digitalRead(di_IRSensor);
  pinMode(di_IRSensor, INPUT);
  pinMode(DI_encoderPin1, INPUT); 
  pinMode(DI_encoderPin2, INPUT);
  digitalWrite(DI_encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(DI_encoderPin2, HIGH); //turn pullup resistor on
  diStepper.setSpeed(750);
  //zero the di motor with the IR sensor
  while(diIR == 1){
    diIR = digitalRead(di_IRSensor);
    diStepper.runSpeed();
  }
  diStepper.setCurrentPosition(2000);
  diStepper.moveTo(0);

  //ASI
  asi.setMaxSpeed(1000);
  asi.setAcceleration(1000);
  int asiIR = digitalRead(asi_IRSensor);
  pinMode(asi_IRSensor, INPUT);
  asi.setSpeed(750);
  //zero the asi motor with the IR sensor
  while(asiIR == 1){
    asiIR = digitalRead(asi_IRSensor);
    asi.runSpeed();
  }
  asi.setCurrentPosition(780);
  asi.moveTo(0);
}

//this is the interpolate function. Used for non-linear gauges to improve accuracy. If anyone has ideas on how to improve it, let me know.
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

void onVoltmetergaugeChange(unsigned int newValue) {
    int tempVal = map(newValue, 0, 65535, 0, -920);
    volt.moveTo(tempVal);
}
DcsBios::IntegerBuffer voltmetergaugeBuffer(0x544c, 0xffff, 0, onVoltmetergaugeChange);

void onTachometergaugeChange(unsigned int newValue) {
    long inLookup[] = {0, 13107, 0.4*65535, 0.6*65535, 0.8*65535, 65535};
    long outLookup[] = {0, 0, 895, 1790, 2685, 3580};
    float tempVal = interpolate(inLookup, outLookup, newValue);
    tacho.moveTo(tempVal);
}
DcsBios::IntegerBuffer tachometergaugeBuffer(0x544e, 0xffff, 0, onTachometergaugeChange);

void onBoostgaugeChange(unsigned int newValue) {
    int tempVal = map(newValue, 0, 65535, 0, 3600);
    boost.moveTo(tempVal);
}
DcsBios::IntegerBuffer boostgaugeBuffer(0x5450, 0xffff, 0, onBoostgaugeChange);

void onRadiatortemperaturegaugeChange(unsigned int newValue) {
    long inLookup[] = {0, 19660, 24258, 28753, 31914, 35831, 40429, 43288, 46760, 48787, 51161, 53320, 65535};
    long outLookup[] = {0, -179, -310, -418, -597, -810, -1135, -1343, -1522, -1760, -1969, -2267, -3580};
    float tempVal = interpolate(inLookup, outLookup, newValue);
    radTemp.moveTo(tempVal);
}
DcsBios::IntegerBuffer radiatortemperaturegaugeBuffer(0x5456, 0xffff, 0, onRadiatortemperaturegaugeChange);

void onOiltemperaturegaugeChange(unsigned int newValue) {
    int tempVal = map(newValue, 0, 65535, 0, 935);
    oilTemp.moveTo(tempVal);
}
DcsBios::IntegerBuffer oiltemperaturegaugeBuffer(0x5454, 0xffff, 0, onOiltemperaturegaugeChange);

void onFuelreservegaugeChange(unsigned int newValue) {
  long inLookup[] = {0, 3000, 65535}; //input from 0-65535
  long outLookup[] = {948, 870, 0}; //output in steps
  float out = interpolate(inLookup, outLookup, newValue);
  fuel.moveTo(out);
}
DcsBios::IntegerBuffer fuelreservegaugeBuffer(0x5458, 0xffff, 0, onFuelreservegaugeChange);

DcsBios::Switch2Pos fuelGauge("FUEL_GAUGE", fuelPin);

void onTrimgaugeChange(unsigned int newValue) {
    int tempVal = map(newValue, 0, 65535, 0, -800);
    nose.moveTo(tempVal);
}
DcsBios::IntegerBuffer trimgaugeBuffer(0x542e, 0xffff, 0, onTrimgaugeChange);

void onGearUpChange(unsigned int newValue) {
  if(newValue == 1){
    digitalWrite(gearUp, LOW);
    digitalWrite(gearDown, HIGH);
  }
  else {
    digitalWrite(gearUp, HIGH);
    digitalWrite(gearDown, LOW);
  }
  
}
DcsBios::IntegerBuffer gearUpBuffer(0x5422, 0x0200, 9, onGearUpChange);

void onTurngaugeChange(unsigned int newValue) {\
  float incomingByte = map(newValue, 0, 65535, 0, 1480);
  turn.moveTo(incomingByte);
}
  
DcsBios::IntegerBuffer turngaugeBuffer(0x544a, 0xffff, 0, onTurngaugeChange);

void onSideslipgaugeChange(unsigned int newValue) {
  long inLookup[] = {0, 13539, 51996, 65535}; //input from 0-65535
  long outLookup[] = {1100, 900, 180, 0}; //output in steps
  float out = interpolate(inLookup, outLookup, newValue);
  slip.moveTo(out);
  }
DcsBios::IntegerBuffer sideslipgaugeBuffer(0x5448, 0xffff, 0, onSideslipgaugeChange);

void onVariometergaugeChange(unsigned int newValue) {
  //convert climb indicator
  float incomingByte = map(newValue, 0, 65535, 0, 3600);
  climb.moveTo(incomingByte);
}

DcsBios::IntegerBuffer variometergaugeBuffer(0x543c, 0xffff, 0, onVariometergaugeChange);

void onAirspeedgaugeChange(unsigned int newValue) {
  long inLookup[] = {0, 7821, 9031, 10556, 11806, 13015, 14378, 15618, 16729, 18248, 19763, 21057, 22185, 23686, 24974, 26259, 27647, 28878, 30259, 31579, 32753, 34119, 35485, 36821, 38109, 39342, 40421, 41835, 43331, 44462, 45725, 47103, 48579, 50009, 51328, 52534, 53980, 55307, 56293, 57817, 58944, 60146, 61603, 65535}; //input from 0-65535
  long outLookup[] = {0, -73, -118, -190, -255, -335, -425, -520, -615, -720, -830, -955, -1075, -1220, -1350, -1475, -1615, -1750, -1890, -2030, -2140, -2270, -2385, -2505, -2600, -2710, -2810, -2910, -3010, -3110, -3215, -3315, -3420, -3520, -3625, -3730, -3820, -3925, -4025, -4130, -4230, -4330, -4430, -4530}; //output in steps
  float out = interpolate(inLookup, outLookup, newValue);
  int asiDelta = out - asi.currentPosition();
  //the asi gauge has some backlash due to the stepper motors gearbox. The function below solves this
  int backlashStepsCCW = 25;
  int backlashStepsCW = 10;
  if(asiDelta > 0 && asiDeltaLast < 0){
    out += backlashStepsCCW;
  }
  else if(asiDelta < 0 && asiDeltaLast > 0){
    out -= backlashStepsCW;
  }
  asiDeltaLast = asiDelta;
  asi.moveTo(out);
}
DcsBios::IntegerBuffer airspeedgaugeBuffer(0x5436, 0xffff, 0, onAirspeedgaugeChange);

void onDigaugeChange(unsigned int newValue) {
  int steps = 5554;
  long target = map(newValue, 0, 65535, 0, steps);
  int delta = target - diStepper.currentPosition();
  //because the di has to be able to rotate indefintely the movement must follow the same logic. Using modulu operator makes this easier
  int move = (delta) % (steps);
  if(move < -steps/2){
    move += steps;
  }
  else if(move > steps/2){
    move -= steps;
  }
  diStepper.move(move);
}
DcsBios::IntegerBuffer digaugeBuffer(0x5446, 0xffff, 0, onDigaugeChange);

DcsBios::RotaryEncoder di("DI", "-3200", "+3200", DI_encoderPin2, DI_encoderPin1);

DcsBios::Switch2Pos blower("BLOWER", supercharger);

DcsBios::LED chargerSecGearLight(0x5422, 0x0800, superchargerLED);

DcsBios::LED fuelPressLight(0x5422, 0x0100, fuelLED);

DcsBios::Switch2Pos gunsightSwitch("GUNSIGHT_SWITCH", gunsightSwitchPin);

DcsBios::Switch2Pos navLights("NAV_LIGHTS", navigationSwitch);

DcsBios::Switch2Pos magneto0("MAGNETO0", magneto1Pin);

DcsBios::Switch2Pos magneto1("MAGNETO1", magneto2Pin);

void loop() {
  DcsBios::loop();
  volt.run();
  tacho.run();
  boost.run();
  radTemp.run();
  oilTemp.run();
  fuel.run();
  nose.run();
  turn.run();
  slip.run();
  climb.run();
  asi.run();
  diStepper.run();
}
