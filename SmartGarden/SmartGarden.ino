/*
   This system makes a pump work 2 times per day(in early morning and sunset, which is determined using an LDR sensor) if the temperature is low, otherwise, it would work 3 times.
   Every time before opening the water it checks the moisture of the soil that it is below 18%.
   If it rains, the next time of watering will be canceled.
   Led would be turned on at night.
   Time and sensors values would be displayed on an LCD.

   When we turn on the pump, we write a 0 on its pin. We use a relay module with low level trigger.
*/

#include <LiquidCrystal.h>

#define ldr A5
#define led 13
#define tmp A4
#define soilSens A3
#define pump 8
#define rainSens A2

//Define some constants
#define earlyTimeLDRmin 550
#define earlyTimeLDRmax 650
#define sunSetTimeLDRmin 450
#define sunSetTimeLDRmax 550
#define tmpThreshold 40
#define rainIndicationVal 500
#define nightIndication 500

//Define lcd constants
#define rs 2
#define e 3
#define d4 4
#define d5 5
#define d6 6
#define d7 7

LiquidCrystal lcd (rs, e, d4, d5, d6, d7);

long lastTime = -24L * 60L * 60L * 1000L - 1;
byte waterFlag = 1;
byte timeFlag = 0; // 0 for morning and 1 for evening
byte sensIndex = 0;

void pumping(byte state);
void updateLCD();

void setup() {
  pinMode(led, OUTPUT);
  pinMode(pump, OUTPUT);
  lcd.begin(16, 2);

}

void loop() {
  if (analogRead(ldr) < earlyTimeLDRmax && analogRead(ldr) > earlyTimeLDRmin)
  {
    if (waterFlag)
    {
      pumping(1);
    }
    while (analogRead(ldr) < earlyTimeLDRmax && analogRead(ldr) > earlyTimeLDRmin) //to wait until this time is finished to toggle the waterFlag again
    {
      updateLCD();
    }
    waterFlag = 1;
  }
  else if (analogRead(ldr) < sunSetTimeLDRmax && analogRead(ldr) > sunSetTimeLDRmin)
  {
    if (waterFlag)
    {
      pumping(1);
    }
    while (analogRead(ldr) < sunSetTimeLDRmax && analogRead(ldr) > sunSetTimeLDRmin) //to wait until this time is finished to toggle the waterFlag again
    {
      updateLCD();
    }
    waterFlag = 1;
  }
  else if (analogRead(tmp) * 500 / 1024.0 > tmpThreshold && millis() - lastTime > 24L * 60L * 60L * 1000L) //if the temperature is HIGH and last time was before 24 hours or more
  {
    if (waterFlag)
    {
      pumping(1);
    }
    while (analogRead(tmp) * 500 / 1024.0 > tmpThreshold && millis() - lastTime > 24L * 60L * 60L * 1000L) //to wait until this time is finished to toggle the waterFlag again
    {
      updateLCD();
    }
    lastTime = millis(); //updating the lastTime variable value
    waterFlag = 1;
  }
  else if ((1 - analogRead(soilSens) / 1024.0) < .05) // if the soil is too dry
  {
    pumping(1);
  }
  else //turning off the pump
  {
    pumping(0);
  }

  if (analogRead(rainSens) < rainIndicationVal)
  {
    waterFlag = 0; //clearing the waterFlag to cancel the next time of watering
  }

  if (analogRead(ldr) < nightIndication)
  {
    digitalWrite(led, 1);
    timeFlag = 1;
  }
  else
  {
    digitalWrite(led, 0);
    timeFlag = 0;
  }

  updateLCD();

}

void pumping(byte state)
{
  /*This void function controls the pump, that it would work only if the moisture is less than 18%.
     Arguments: You should pass 1 for turning on the pump, and 0 for turning it off, otherwise, nothing happens.
  */
  if ((1 - analogRead(soilSens) / 1024.0) < .18 && state == 1)
  {
    while ((1 - analogRead(soilSens) / 1024.0) < .18) //continue watering until the moisture increases
    {
      digitalWrite(pump, 0);
      updateLCD(); //to continue updating the lcd while watering
    }
  }
  else if (!state) //if the state is 0
  {
    digitalWrite(pump, 1);
  }
}

void updateLCD()
{
  /*
     This function displays the time and sensors values on the LCD.
     It is using a global variable sensIndex to change the sensor which is displayed every 2s.
  */
  lcd.clear();
  lcd.print("Time: ");
  if (timeFlag)
  {
    lcd.print("evening");
  }
  else
  {
    lcd.print("morning");
  }
  lcd.setCursor(0, 1);
  if (sensIndex == 0)
  {
    lcd.print("LDR: ");
    lcd.print(analogRead(ldr));
  }
  else if (sensIndex == 1)
  {
    lcd.print("TMP: ");
    lcd.print(analogRead(tmp) * 500 / 1024.0);
  }
  else if (sensIndex == 2)
  {
    lcd.print("Moisture: ");
    lcd.print(1 - analogRead(soilSens) / 1024.0);
  }
  else if (sensIndex == 3)
  {
    lcd.print("RainSens: ");
    lcd.print(analogRead(rainSens));
  }
  sensIndex++;
  if (sensIndex == 4)
  {
    sensIndex = 0;
  }
  delay(2000);
}
