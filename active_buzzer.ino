#include "LiquidCrystal.h"
#include <Wire.h>
#include "RTClib.h"
#include "IRremote.h"
#include "IR.h"
#include "IRremoteInt.h"

IRrecv irrecv(RECEIVER);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 6, 12);

int buzzerPin = 5;
int j=3;
int k=12;
RTC_DS1307 RTC;
int alarm_hour = 4;
int alarm_minute = 0;
int is_alarm = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(buzzerPin, OUTPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // init ir
  irrecv.enableIRIn();

  Wire.begin();
  RTC.begin();
  // RTC.adjust(DateTime(__DATE__, __TIME__));
}

void loop() {
  // reset lcd
  lcd.clear();
  lcd.setCursor(0,0);

  // get curren time
  DateTime now = RTC.now(); 

  // parse current time
  int hours = now.hour();
  int minutes = now.minute();
  int seconds = now.second(); 

  // show current time
  lcd.print("Time: ");

  if (hours < 10) {
    lcd.print(0);
    lcd.print(hours);
  } else {
    lcd.print(hours);
  }
  lcd.print(":");
  if (minutes < 10) {
    lcd.print(0);
    lcd.print(minutes);
  } else {
    lcd.print(minutes);
  }
  lcd.print(":");
  if (seconds < 10) {
    lcd.print(0);
    lcd.print(seconds);
  } else {
    lcd.print(seconds);
  }

  // show current alarm set
  lcd.setCursor(0,1); 
  lcd.print("Alarm: ");
  if (alarm_hour < 10) {
    lcd.print(0);
    lcd.print(alarm_hour);
  } else {
    lcd.print(alarm_hour);
  }
  lcd.print(":");
  if (alarm_minute < 10) {
    lcd.print(0);
    lcd.print(alarm_minute);
  } else {
    lcd.print(alarm_minute);
  }
  lcd.print(":00");

  int tmpValue;
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    for (int i = 0; i < 23; i++)
    {
      if ((keyValue[i] == results.value) && (i<KEY_NUM))
      {
        tmpValue = results.value;

        if(results.value == KEY_DOWN) {
          if (alarm_hour == 0) {
            alarm_hour = 23;
          } else {
            alarm_hour = alarm_hour - 1;
          }
        }

        if(results.value == KEY_UP) {
          if (alarm_hour == 23) {
            alarm_hour = 0;
          } else {
            alarm_hour = alarm_hour + 1;
          }
        }

        if(results.value == KEY_VOL_DE)
        {
          if (alarm_minute - 10 < 0 && alarm_hour > 0) {
            alarm_minute = alarm_minute + 50;
            alarm_hour = alarm_hour - 1;
          } else if (alarm_minute - 10 < 0 && alarm_hour == 0) {
            alarm_minute = alarm_minute + 50;
            alarm_hour = 23;
          } else {
            alarm_minute = alarm_minute - 10;
          }
        }
        if(results.value == KEY_VOL_ADD)
        {
          if (alarm_minute + 10 > 59 && alarm_hour < 23) {
            alarm_minute = alarm_minute - 50;
            alarm_hour = alarm_hour + 1;
          } else if (alarm_minute + 10 > 59 && alarm_hour == 23) {
            alarm_minute = alarm_minute - 50;
            alarm_hour = 0;
          } else {
            alarm_minute = alarm_minute + 10;
          }
        }

        if(results.value == KEY_FAST_FORWARD)
        {
          if (alarm_minute == 59 && alarm_hour < 23) {
            alarm_minute = 0;
            alarm_hour = alarm_hour + 1;
          } else if (alarm_minute == 59 && alarm_hour == 23) {
            alarm_minute = 0;
            alarm_hour = 0;
          } else {
            alarm_minute = alarm_minute + 1;
          }
        }

        if(results.value == KEY_FAST_BACK)
        {
          if (alarm_minute == 0 && alarm_hour > 0) {
            alarm_minute = 59;
            alarm_hour = alarm_hour - 1;
          } else if (alarm_minute == 1 && alarm_hour == 0) {
            alarm_minute = 59;
            alarm_hour = 23;
          } else {
            alarm_minute = alarm_minute - 1;
          }
        }

        if(results.value == KEY_POWER) {
          is_alarm = 0;
        }
      }
      else if(REPEAT==i)
      {
        results.value = tmpValue;
      }
    }
    irrecv.resume(); // receive the next value
  }

  if (hours == alarm_hour && minutes == alarm_minute && seconds == 0) {
    is_alarm = 1;
  }

  if (is_alarm == 1) {
    siren();
  }

  delay(100);
}

void siren() {
  // Whoop up
  for(int hz = 440; hz < 1000; hz+=25){
    tone(buzzerPin, hz, 50);
    delay(5);
  }
  // Whoop down
  for(int hz = 1000; hz > 440; hz-=25){
    tone(buzzerPin, hz, 50);
    delay(5);
  }
}
