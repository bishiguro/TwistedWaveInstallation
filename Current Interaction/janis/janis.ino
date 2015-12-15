//Shrinidhi Thirumalai, Bonnie Ishiguro, Charlie Mouton, Jason Yan
//Interaction for 12 paddles moving in response to your motion

//----------------------------------------------------------------------------Libraries and Packages----------------------------------------------------------------------
#include <Servo.h>
#include <setup.h>
#include <Time.h>
#include <TimeLib.h>
//#include <DS1307RTC.h>
//-----------------------------------------------------------------------------Initializations---------------------------------------------------------------------------

//Already Included: POS_MAX, NUM_SERVOS, NUM_SENSORS, servo_pins, sensor_pins
int servo_pins[NUM_SERVOS] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
int sensor_pins[NUM_SENSORS] = {A5, A4, A3, A2, A1, A0};

//State Variables
Servo servos[NUM_SERVOS];
State janis;
bool sensor_vals[NUM_SENSORS];
bool sensing;

//------------------------Setup----------------------------------------
State initializeJanis() {
  State janis;
  memset(janis.pos, 0, NUM_SERVOS);
  memset(janis.is_reverse, 0, NUM_SERVOS);
  return janis;
}

// void setupRTC() {
//   while (!Serial) ; // wait until Arduino Serial Monitor opens
//   setSyncProvider(RTC.get);   // the function to get the time from the RTC
//   if(timeStatus()!= timeSet) 
//      Serial.println("Unable to sync with the RTC");
//   else
//      Serial.println("RTC has set the system time");  
// }

void attachServos() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].attach(servo_pins[i]);
    servos[i].write(0);
  }
}
//--------------Get Sensor Input--------------------------------------
bool ifDetect() {
  //returns true if sensor_vals contains at least one true
  for (int i = 0; i <NUM_SENSORS; i++)
    if (sensor_vals[i]) return true;
  return false;
}

void getSensorInput() {
  //writes bool of if sensor sensed value or not, onto sensor_vals
  int threshold = 200;
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensor_vals[i] = (analogRead(sensor_pins[i]) >= threshold);
  }
}
//----------------Update State--------------------------------------------
void updateSensorState( void (*f)(int) ) {
  //if a sensor sees something, run the given control function
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (sensor_vals[i])
      (*f)(i);
  }
}

void updateTimeState() {
  printTime();
  if(timeStatus() == timeSet) {
    hourPaddle();
    fiveMinutePaddle();
  }
  else {
    Serial.println("The time has not been set.");

  }
}
//------------------Display State------------------------------------------
void displayState() {
  //write state to servos
  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].write(janis.pos[i]);
  }
}

//-----------------------------------------------------------------Main Ardiono Loops---------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(9600);
  //setupRTC();

  memset(sensor_vals, 0, NUM_SENSORS);
  janis = initializeJanis();
  attachServos();
  calibrate();
  //displayState();
}

void calibrate() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    // janis[i].pos = 0;
    servos[i].write(0);
  }
}

void printState() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    Serial.println(janis.pos[i]);
  }
  Serial.println("===============================");
}
void loop()
{
  getSensorInput();
  
  for (int i = 0; i<NUM_SENSORS; i++) {
    Serial.print(sensor_vals[i]); Serial.print("| ");
  }
  Serial.println("");
  

  if (ifDetect()) {
    sensing = true;
    updateSensorState(sweepBackForthControl);
    // printState();
  }
  else if (sensing) {
    calibrate();
  }
  else
    sensing = false;
    updateTimeState();
   displayState();
}