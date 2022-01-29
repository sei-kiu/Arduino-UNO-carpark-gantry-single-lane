#include <Arduino.h>

#include "Ultrasonic.h"
#include <Servo.h>
#include "rgb_lcd.h"

#define ULTRASONIC_DIST_THRESHOLD 10
#define SERVO_ANGLE_OPEN 60
#define SERVO_ANGLE_CLOSE 0

Ultrasonic ultrasonicEntry(2);
Ultrasonic ultrasonicExit(4);
Servo myServo;
rgb_lcd lcd;

int count = 0;

enum gantryStates
{
  idle,
  entryRequested,
  entryGrantedWaitTillExitSensorReached,
  entryGrantedWaitTillExitSensorPassed,
  exitRequested,
  exitGrantedWaitTillEntrySensorReached,
  exitGrantedWaitTillEntrySensorPassed
};

gantryStates gantryState = idle;

void setup()
{
  // put your setup code here, to run once:

  myServo.attach(6);
  myServo.write(SERVO_ANGLE_CLOSE); // Close barrier
  delay(15);                        //  Give some time for servo to move

  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("   Sei Kiu's    ");
  lcd.setCursor(0, 1);
  lcd.print("    carpark     ");
  delay(1500);
  lcd.clear();
}

void loop()
{
  // put your main code here, to run repeatedly:

  long ultrasonicEntryRangeInCm;
  long ultrasonicExitRangeInCm;

  ultrasonicEntryRangeInCm = ultrasonicEntry.MeasureInCentimeters();
  ultrasonicExitRangeInCm = ultrasonicExit.MeasureInCentimeters();

  lcd.setCursor(0, 0);
  lcd.print("IN:    ");
  lcd.setCursor(3, 0);
  lcd.print(ultrasonicEntryRangeInCm);
  lcd.setCursor(8, 0);
  lcd.print("OUT:    ");
  lcd.setCursor(12, 0);
  lcd.print(ultrasonicExitRangeInCm);
  lcd.setCursor(0, 1);
  lcd.print("Count:    ");
  lcd.setCursor(7, 1);
  lcd.print(count);

  switch (gantryState)
  {
  case idle:
    if (ultrasonicExitRangeInCm < ULTRASONIC_DIST_THRESHOLD)
    {
      gantryState = exitRequested;
    }
    else if (ultrasonicEntryRangeInCm < ULTRASONIC_DIST_THRESHOLD)
    {
      gantryState = entryRequested;
    }
    break;
  case entryRequested:
    if (ultrasonicExitRangeInCm > ULTRASONIC_DIST_THRESHOLD)
    {                                  // if exit path is clear
      myServo.write(SERVO_ANGLE_OPEN); // Open barrier
      delay(15);                       // Give some time for servo to move

      count++;

      gantryState = entryGrantedWaitTillExitSensorReached;
    }
    break;
  case entryGrantedWaitTillExitSensorReached:
    if (ultrasonicExitRangeInCm < ULTRASONIC_DIST_THRESHOLD)
    {
      gantryState = entryGrantedWaitTillExitSensorPassed;
    }
    break;
  case entryGrantedWaitTillExitSensorPassed:
    if (ultrasonicExitRangeInCm > ULTRASONIC_DIST_THRESHOLD)
    {
      myServo.write(SERVO_ANGLE_CLOSE); // Close barrier
      delay(15);                        //  Give some time for servo to move

      gantryState = idle;
    }
    break;
  case exitRequested:
    if (ultrasonicEntryRangeInCm > ULTRASONIC_DIST_THRESHOLD)
    {                                  // if entry path is clear
      myServo.write(SERVO_ANGLE_OPEN); // Open barrier
      delay(15);                       // Give some time for servo to move

      count--;

      gantryState = exitGrantedWaitTillEntrySensorReached;
    }
    break;
  case exitGrantedWaitTillEntrySensorReached:
    if (ultrasonicEntryRangeInCm < ULTRASONIC_DIST_THRESHOLD)
    {
      gantryState = exitGrantedWaitTillEntrySensorPassed;
    }
    break;
  case exitGrantedWaitTillEntrySensorPassed:
    if (ultrasonicEntryRangeInCm > ULTRASONIC_DIST_THRESHOLD)
    {
      myServo.write(SERVO_ANGLE_CLOSE); // Close barrier
      delay(15);                        //  Give some time for servo to move

      gantryState = idle;
    }
    break;
  default:
    gantryState = idle;
    break;
  }

  delay(200);
}