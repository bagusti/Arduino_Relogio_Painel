// Program to Display Date, Time, Temp. and Humidity with Module MAX7219 
//
// Author: Marcos Bernardo - xmarcos.aab@gmail.com
//
// Sensors: MAX7219 - Display LED
//          DHT11   - Sensor Temp and Humid.
//          DS1307  - RTC Real Time Clock
//          Arduino - Nano controller
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//
// Version 2.0 - 08-2019
//   Add Adjust time.
/////////////////////////////

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
//RTC DS1307
#include <DS1307.h>
//Lib  Sensor DHT11 Temp. Humid.
#include <dht.h>

//RTC DS1307 pins A4 and A5 - Arduino nano
DS1307 rtc(A4, A5);

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10
#define DHT_PIN   5
#define BtnAdjustMin 7
#define BtnAdjustHor 9

//Flag Display sequenci
int flag = 0;
bool btnStateMinBefore = HIGH;
bool btnStateHorBefore = HIGH;
unsigned long btnDebounce;
unsigned int Debounce = 100;

// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

//Sensor 
dht DHT ;

void xAdjustHours();
void AdjustMinutes();

void setup(void)
{
  P.begin();
  //Init Clock
  rtc.halt(false);
  //pin SQW/Out
  rtc.setSQWRate(SQW_RATE_1);
  rtc.enableSQW(true);
  Serial.begin(9600);
  //Set Time 
  //rtc.setTime(20,01,40);
  //Set Date
  // rtc.setDate(31,8,2019);
  // Set Day
  // rtc.setDOW(6);
  // Buttons
  pinMode(BtnAdjustMin, INPUT_PULLUP);
  pinMode(BtnAdjustHor, INPUT_PULLUP);
}

void loop(void)
{
  //Buttons to Adjust
  bool btnStateMin = digitalRead(BtnAdjustMin);
  bool btnStateHor = digitalRead(BtnAdjustHor);
  
  // if button pressed
  if ((millis() - btnDebounce ) > Debounce){
    //Minutes
    if (!btnStateMin && btnStateMinBefore){
      AdjustMinutes();
      btnDebounce = millis();
    }
    //Hours
    if (!btnStateHor && btnStateHorBefore){
      xAdjustHours();
      btnDebounce = millis();
    }
  }

  // If finished displaying message 
  if (P.displayAnimate()) 
  {
     // Gets Time, Date and day
     char hora[5];
     strcpy( hora, rtc.getTimeStr(FORMAT_SHORT));
     char data[10]; 
     strcpy( data, rtc.getDateStr());
     char dia [15];
     strcpy(dia, rtc.getDOWStr(FORMAT_SHORT));
     
     switch (flag) {
        case 0:
        {
          //Display Time
          P.displayText(hora , PA_CENTER,180, 2500, PA_MESH, PA_MESH);
          flag++;
          break;
        }
        case 1:
        {
          //Display Temp and Humidity
          int chk =  DHT.read11(DHT_PIN);
          delay(50);
          char xtemp[10],
               humd[6],
               txt[40] = "";
               
          //Create Display  String    
          dtostrf(DHT.temperature,3,0, xtemp);
          strcat(xtemp , "C");
          dtostrf(DHT.humidity,3, 0, humd);
          strcat(humd , "%");
          strcat(txt, xtemp);
          strcat(txt, " - ");
          strcat(txt, humd);    
          P.displayText(txt , PA_LEFT, 50, 100, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
          flag++;
          break;
        }
        case 2:
        {
          //Display Time
          P.displayText(hora , PA_CENTER,180, 2500, PA_MESH, PA_MESH);
          flag++;
          break;
        }
        case 3:
        {
          //Display Date
          P.displayText(data , PA_LEFT, 50, 100, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
          flag++;
          break;
        }
        case 4:
        {
          //Display Time
          P.displayText(hora , PA_CENTER,180, 2500, PA_MESH, PA_MESH);
          flag++;
          break;
        }
        default:
        {
          //Display dy of week
          P.displayText(dia , PA_LEFT, 50, 100, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
          flag = 0;
          break;
        }
     }
     P.displayReset();  // Reset and display it again
  }
  //Estado
  btnStateMinBefore = btnStateMin;
  btnStateHorBefore = btnStateHor;
}

//Adjust Minutes
void AdjustMinutes(){

  //Get Time
  Time nowHour = rtc.getTime();
  unsigned int Hours = nowHour.hour;
  unsigned Minutes = nowHour.min ;
  unsigned long bDebounce;
  unsigned long Init ;
  char xhora[5];
  bool stateBefore;
  strcpy( xhora, rtc.getTimeStr(FORMAT_SHORT));

  P.displayText(xhora, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  
  //While buttons Press
  while(!digitalRead(BtnAdjustMin)){
      //Time to increment
      if  ((millis() - Init) > 500)
      { 
        Minutes = (Minutes == 59 ) ? 0 : Minutes+1;
        Init = millis();
      }
      String aux;
      if (String(Hours).length() == 1){ 
        aux = "0";
        aux += String(Hours); 
      }else{
         aux  = String(Hours);
      }
      aux += ":";
      if (String(Minutes).length() == 1){ aux += "0"; }
      aux += String(Minutes);
      P.displayText(aux.c_str(), PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
      //Lock while is animate
      while(!P.displayAnimate());
  }
  P.displayReset();
  //Set new minute
  rtc.setTime(Hours,Minutes,00);
}

//Adjust Hours
void xAdjustHours(){

  Time nowHour = rtc.getTime();
  unsigned int Hours = nowHour.hour;
  unsigned Minutes = nowHour.min ;
  unsigned long bDebounce;
  unsigned long Init ;
  char xhora[5];
  bool stateBefore;
  strcpy( xhora, rtc.getTimeStr(FORMAT_SHORT));

  P.displayText(xhora, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  
  //While buttons press
  while(!digitalRead(BtnAdjustHor)){
     
      if  ((millis() - Init) > 500)
      { 
        Hours = (Hours == 23 ) ? 0 : Hours+1;
        Init = millis();
      }
      String aux;
      if (String(Hours).length() == 1){ 
        aux = "0";
        aux += String(Hours); 
      }else{
         aux  = String(Hours);
      }
      aux += ":";
      if (String(Minutes).length() == 1){ aux += "0"; }
      aux += String(Minutes);
      P.displayText(aux.c_str(), PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
      while(!P.displayAnimate());
  }
  P.displayReset();
  //Set new Hour
  rtc.setTime(Hours,Minutes,00);
}
