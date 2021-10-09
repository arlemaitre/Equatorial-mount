#include <SoftwareSerial.h>
#include <Arduino.h>

#define MOTOR_STEPS 200
#define RPM 120

#define DIR 8
#define STEP 9
#define SLEEP 6

#include "DRV8825.h"
#define MODE0 10
#define MODE1 11
#define MODE2 12
DRV8825 stepper(MOTOR_STEPS, DIR, STEP, SLEEP, MODE0, MODE1, MODE2);


SoftwareSerial HC06(2, 3);
String messageRecu;

unsigned long currentTime = 0;
unsigned long previousTimeMotor = 0, previousTimeDARV70 = 0, previousTimeDARV130 = 0, intervall = 0;
bool desarmMotor = true;
bool stopMotor = false;
bool DARV70 = false;
bool DARV130 = false;
bool firststepDARV = true;
void setup() {
  stepper.begin(RPM);
  stepper.setEnableActiveState(LOW);
  stepper.enable();
  Serial.begin(9600);
  HC06.begin(9600);
}

void loop()
{
  currentTime = millis();
  if (desarmMotor) {
    stepper.disable();
  } else {
    stepper.enable();
  }
  if (!stopMotor) {
    if ((currentTime - previousTimeMotor) >= intervall) {
      Serial.println((currentTime - previousTimeMotor));
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

    desarmMotor = false;
    stopMotor = false;
    DARV70 = false;
    DARV130 = false;
    if (pole == "n") {
      switch (delayRcv) {
        case 1110:
          HC06.write("Désarmé \n");
          desarmMotor = true;
          break;
        case 1111:
          HC06.write("Stop \n");
          stopMotor = true;
          break;
        case 1112:
          HC06.write("darv 130sec...wait \n");
          DARV130 = true;
          previousTimeDARV130 = currentTime;
          break;
        case 1113:
          HC06.write("darv 70sec...wait \n");
          DARV70 = true;
          previousTimeDARV70 = currentTime;
          break;
        default:
          intervall = delayRcv;
          if (intervall == 3366) {
            HC06.write("Sidéral \n");
          } else if ( intervall == 1683) {
            HC06.write("Sidéralx2 \n");
          }
          break;
      }
    } else {
      Serial.println("Sud");
      switch (delayRcv) {
        case 1111:
          stopMotor = true;
          break;
        case 1112:
          break;
        case 1113:
          break;
        default:
          intervall = delayRcv;
          break;
      }
    }
    messageRecu = "";

  }

  if (DARV70) {
    if ((currentTime - previousTimeDARV70) >= 70000) {
      HC06.write("Darv70sec finish \n");
      stopMotor = true;
      DARV70 = false;
      firststepDARV = true;
    } else if ((currentTime - previousTimeDARV70) >= 40000) {
      stopMotor = false;
      intervall = 1683;
    } else if ((currentTime - previousTimeDARV70) >= 10000) {
      stopMotor = true;
    } else if (firststepDARV) {
      intervall = 3366;
      firststepDARV = false;
    }
  }
  if (DARV130) {
    if ((currentTime - previousTimeDARV130) >= 130000) {
      HC06.write("Darv130sec finish \n");
      stopMotor = true;
      DARV130 = false;
      firststepDARV = true;
    } else if ((currentTime - previousTimeDARV130) >= 70000) {
      stopMotor = false;
      intervall = 1683;
    } else if ((currentTime - previousTimeDARV130) >= 10000) {
      stopMotor = true;
    } else if (firststepDARV) {
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
