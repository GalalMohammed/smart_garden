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

int lastTime = -24 * 60 * 60 * 1000 - 1;
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
    if (!(analogRead(ldr) < earlyTimeLDRmax && analogRead(ldr) > earlyTimeLDRmin))
    {
      waterFlag = 1;
    }
  }
  else if (analogRead(ldr) < sunSetTimeLDRmax && analogRead(ldr) > sunSetTimeLDRmin)
  {
    if (waterFlag)
    {
      pumping(1);
    }
    if (!(analogRead(ldr) < sunSetTimeLDRmax && analogRead(ldr) > sunSetTimeLDRmin))
    {
      waterFlag = 1;
    }
  }
  else if (analogRead(tmp) * 500 / 1024.0 > tmpThreshold && millis() - lastTime > 24 * 60 * 60 * 1000)
  {
    if (waterFlag)
    {
      pumping(1);
      lastTime = millis();
    }
    if (!(analogRead(tmp) * 500 / 1024.0 > tmpThreshold && millis() - lastTime > 24 * 60 * 60 * 1000))
    {
      waterFlag = 1;
    }
    
  }
  else if (analogRead(soilSens) / 1024.0 > .82)
  {
    digitalWrite(pump, 0);
  }
  else
  {
    pumping(0);
  }

  if (analogRead(rainSens) < rainIndicationVal)
  {
    waterFlag = 0;
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
  if (analogRead(soilSens) / 1024.0 < .18 && analogRead(soilSens) / 1024.0 > .2 && state)
  {
    while(analogRead(soilSens) / 1024.0 < .18 && analogRead(soilSens) / 1024.0 > .2 && state)
    {
      state = 0;
      digitalWrite(pump, state);
      updateLCD();
    }
  }
  else
  {
    state = 1;
    digitalWrite(pump, state);
  }
}

void updateLCD()
{
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
