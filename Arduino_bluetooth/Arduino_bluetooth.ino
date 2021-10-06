#include <SoftwareSerial.h>
#include <Arduino.h>

#define MOTOR_STEPS 200
#define RPM 120

#define DIR 8
#define STEP 9
#define SLEEP 13

#include "DRV8825.h"
#define MODE0 10
#define MODE1 11
#define MODE2 12
DRV8825 stepper(MOTOR_STEPS, DIR, STEP, SLEEP, MODE0, MODE1, MODE2);


SoftwareSerial HC06(2, 3);
String messageRecu;

unsigned long currentTime = 0;
unsigned long previousTimeMotor = 0, previousTimeDARV30 = 0, previousTimeDARV120 = 0, intervall;
bool stopMotor = false;
bool DARV30 = false;
bool DARV120 = false;
bool firststepDARV = true;
void setup() {
  stepper.begin(RPM);
  stepper.enable();
  Serial.begin(9600);
  HC06.begin(9600);
}

void loop()
{
  currentTime = millis();
  if (stopMotor) {
    if ((currentTime - previousTimeMotor) > intervall) {
      Serial.println("step !");
      previousTimeMotor = currentTime;
      stepper.setMicrostep(32);
      stepper.move(1);
    }
  }
  if (HC06.available())
  {
    char c = HC06.read();
    messageRecu += c;
  }

  if (messageRecu.length() >= 6)
  {
    int delayRcv = getValue(messageRecu, ':', 0).toInt();
    Serial.println(delayRcv);
    String pole = getValue(messageRecu, ':', 1);
    Serial.println(pole);
    stopMotor = true;
    DARV30 = false;
    DARV120 = false;
    if (pole == "n") {
      Serial.println("Nord");
      switch (delayRcv) {
        case 1111:
          Serial.println("stop");
          stopMotor = false;
          break;
        case 1112:
          Serial.println("DARV 2min");
          DARV120 = true;
          previousTimeDARV120 = currentTime;
          break;
        case 1113:
          Serial.println("DARV 30sec");
          DARV30 = true;
          previousTimeDARV30 = currentTime;
          break;
        default:
          intervall = delayRcv;
          Serial.println(intervall);
          break;
      }
    } else {
      Serial.println("Sud");
      switch (delayRcv) {
        case 1111:
          Serial.println("stop");
          stopMotor = false;
          break;
        case 1112:
          Serial.println("DARV 2min");
          break;
        case 1113:
          Serial.println("DARV 30sec");
          break;
        default:
          intervall = delayRcv;
          Serial.println(intervall);
          break;
      }
    }
    messageRecu = "";

  }

  if (DARV30) {
    if ((currentTime - previousTimeDARV30) > 30000) {
      Serial.println("DARV_end");
      stopMotor = true;
      DARV30 = false;
      firststepDARV = true;
    } else if ((currentTime - previousTimeDARV30) > 15000) {
      Serial.println("DARV_sidéralx2");
      stopMotor = true;
      intervall = 1683;
    } else if ((currentTime - previousTimeDARV30) > 5000) {
      Serial.println("DARV_stop");
      stopMotor = false;
    } else if (firststepDARV) {
      Serial.println("DARV_sidéral");
      intervall = 3366;
      firststepDARV = false;
    }
  }
  if (DARV120) {
    if ((currentTime - previousTimeDARV120) > 120000) {
      Serial.println("DARV_end");
      stopMotor = true;
      DARV120 = false;
      firststepDARV = true;
    } else if ((currentTime - previousTimeDARV120) > 60000) {
      Serial.println("DARV_sidéralx2");
      stopMotor = true;
      intervall = 1683;
    } else if ((currentTime - previousTimeDARV120) > 5000) {
      Serial.println("DARV_stop");
      stopMotor = false;
    } else if (firststepDARV) {
      Serial.println("DARV_sidéral");
      intervall = 3366;
      firststepDARV = false;
    }
  }

}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
