/*
 * WateringPot.hpp
 *
 *  Created on: May 15, 2020
 *      Author: Franziska Wegner
 */
 
#include <Servo.h>

/***********************************************************************
************************************************************************
* CONSTANTS AND PORTS
************************************************************************
************************************************************************/
#define DEBUGING_ON

bool waterPlant_                        = false;
const int PIN_LED_                      = LED_BUILTIN; // Port 13

// Servo specific stuff
Servo     myservo_; // create servo object to control servo; we use tower pro, micro servo 9g sg90
const int PIN_SERVO_                    = 9; // Port 9 Servo control output

int const PLANT_SERVO_POSITION_         = 140;
int const NORMAL_SERVO_POSITION_        = 90;
int const PERSON_SERVO_POSITION_        = 35;
int const MOVE_TOWARDS_PLANT_SPEED_     = 5;
int const MOVE_TOWARDS_OBJECT_SPEED_    = 50;
int const MOVE_TOWARDS_START_SPEED_     = 7;

// Plant Moisture sensor stuff
int const PIN_PLANT_MOISTURE_SENSOR_    = A0;
int const PLANT_THRESHOLD_              = 400; 

// Watering Pot Moisture sensor stuff
int const PIN_WATERING_POT_MOISTURE_SENSOR_ = A1;
int const PIN_RELAYS_                    = 7;
// Fillings
int const WATERING_POT_THRESHOLD_1_1_   = 291;
int const WATERING_POT_THRESHOLD_3_4_   = 301;
int const WATERING_POT_THRESHOLD_1_2_   = 314;
int const WATERING_POT_THRESHOLD_1_4_   = 353;
// Actual used Threshold
int const WATERING_POT_THRESHOLD_       = 310; // WATERING_POT_THRESHOLD_1_2_;

// Microphone Sensor Module - KY-037
int const PIN_MICROPHONE_ANALOG_        = A2;
int const PIN_MICROPHONE_DIGITAL_       = 8;
int const MICROPHONE_THRESHOLD_         = 0;

/***********************************************************************
************************************************************************
* MICROPHONE SENSOR METHODS
************************************************************************
************************************************************************/

bool ReachedMicrophoneThreshold ()
{
  if ( digitalRead ( PIN_MICROPHONE_DIGITAL_ ) == HIGH ) 
  {
#ifdef DEBUGING_ON
    Serial.println ( "Microphone DIGITAL Sensor Value TRUE! " );
#endif
    return true;
  } 
  return false;
}

/***********************************************************************
************************************************************************
* MOISTURE SENSOR METHODS
************************************************************************
************************************************************************/

void PrintMoistureSensorValues ()
{
#ifdef DEBUGING_ON
  Serial.print ( "Plant MOISTURE Sensor Value: " );
  Serial.println ( analogRead ( PIN_PLANT_MOISTURE_SENSOR_) );
#endif
}

void PrintWaterPotMoistureSensorValues ()
{
#ifdef DEBUGING_ON
  Serial.print ( "Water Pot MOISTURE Sensor Value: " );
  Serial.println ( analogRead ( PIN_WATERING_POT_MOISTURE_SENSOR_) );
#endif
}

bool PlantNeedsWater ()
{
  return ( analogRead ( PIN_PLANT_MOISTURE_SENSOR_) >= PLANT_THRESHOLD_ );
}

bool WaterPotNeedsWater ()
{
  return ( analogRead ( PIN_WATERING_POT_MOISTURE_SENSOR_) >= WATERING_POT_THRESHOLD_ );
}

/***********************************************************************
************************************************************************
* WATER PUMP METHODS
************************************************************************
************************************************************************/

void StartWaterPump ()
{
  digitalWrite ( PIN_RELAYS_, HIGH );  
}

void StopWaterPump ()
{
  digitalWrite ( PIN_RELAYS_, LOW );
}

/***********************************************************************
************************************************************************
* SERVO METHODS
************************************************************************
************************************************************************/

void MoveServoTowards ( int moveDirection = PERSON_SERVO_POSITION_
                      , int moveSpeed     = MOVE_TOWARDS_OBJECT_SPEED_
                      , int wait          = 1000
                      , int startPosition = NORMAL_SERVO_POSITION_ )
{
  int counter = startPosition;
  if ( moveDirection < startPosition )
  {
      counter = -1* counter;
  }
  
  for ( 
      ; abs(counter) != moveDirection
      ; ++counter ) 
  {
    myservo_.write ( abs(counter) );
    delay ( moveSpeed );
#ifdef DEBUGING_ON
    PrintServoPosition();
#endif
  }
  delay ( wait );
}

void MoveServoTowardsStartPosition ( int moveSpeed      = MOVE_TOWARDS_START_SPEED_
                                   , int wait           = 1000
                                   , int startPosition  = NORMAL_SERVO_POSITION_ )
{
  MoveServoTowards ( NORMAL_SERVO_POSITION_
                   , moveSpeed
                   , wait
                   , myservo_.read() );
}

void PrintServoPosition ()
{
#ifdef DEBUGING_ON
  Serial.print ( "SERVO position: " );
  Serial.println ( myservo_.read() );
#endif
}

/***********************************************************************
************************************************************************
* SETUP
************************************************************************
************************************************************************/
void setup() 
{
  // put your setup code here, to run once:
#ifdef DEBUGING_ON
  Serial.begin ( 9600 ); // Debugging
#endif
  pinMode ( PIN_RELAYS_, OUTPUT );
  pinMode ( PIN_LED_,    OUTPUT );
  pinMode ( PIN_MICROPHONE_DIGITAL_, INPUT );

  // Attache the servo on pin 9 to the servo object
  myservo_.attach ( PIN_SERVO_ );
  myservo_.write  ( NORMAL_SERVO_POSITION_ );
}

/***********************************************************************
************************************************************************
* WATER PUMP BEHAVIOR
************************************************************************
************************************************************************/
void StartWaterPumpBehavior ()
{
  if ( WaterPotNeedsWater() )
  {
    StartWaterPump();
  }
#ifndef DEBUGING_ON
  while ( WaterPotNeedsWater() );
#else
  while ( WaterPotNeedsWater() )
  {
    PrintWaterPotMoistureSensorValues ();
  }
#endif
  StopWaterPump();
  PrintWaterPotMoistureSensorValues ();
}

/***********************************************************************
************************************************************************
* SERVO BEHAVIOR
************************************************************************
************************************************************************/
void StartServoBehavior ()
{
  // Move towards plant
  if ( PlantNeedsWater() )
  {
    MoveServoTowards ( PLANT_SERVO_POSITION_ );
    MoveServoTowardsStartPosition ();
  }
  // Move towards PERSON
  MoveServoTowards ( PERSON_SERVO_POSITION_ );
  MoveServoTowardsStartPosition ();
  PrintServoPosition();
}

/***********************************************************************
************************************************************************
* LED BEHAVIOR
************************************************************************
************************************************************************/
void StartLedBehavior ()
{
  digitalWrite ( PIN_LED_, HIGH );
  delay ( 1000 );
  digitalWrite ( PIN_LED_, LOW );
  delay ( 1000 );
#ifdef DEBUGING_ON  
  Serial.println ( PIN_LED_ );
#endif
}

/***********************************************************************
************************************************************************
* LOOP
************************************************************************
************************************************************************/
void loop() 
{
  if ( ReachedMicrophoneThreshold () )
  {
    StartWaterPumpBehavior ();
    if ( !waterPlant_ )
    {
      PrintWaterPotMoistureSensorValues ();
      MoveServoTowards ( PERSON_SERVO_POSITION_, MOVE_TOWARDS_PLANT_SPEED_ );
      MoveServoTowardsStartPosition ();
      waterPlant_ = true;
    } else {
      PrintMoistureSensorValues();
      // Move towards Plant
      StartWaterPumpBehavior ();
      MoveServoTowards ( PLANT_SERVO_POSITION_ );
      delay(1000);
      MoveServoTowardsStartPosition ();  
      waterPlant_ = false;
    }
  }
//  
  if ( PlantNeedsWater() )
  {
    // Move towards Plant
    StartWaterPumpBehavior ();
    MoveServoTowards ( PLANT_SERVO_POSITION_ );
    delay(1000);
    MoveServoTowardsStartPosition ();
  }
  PrintWaterPotMoistureSensorValues ();
  PrintMoistureSensorValues();
}
    
