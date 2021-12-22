#include <WiFi.h>
#include "time.h"
#include "RTClib.h"
#include <Arduino.h>
RTC_DS3231 rtc;
#include "DHT.h"
#include <Adafruit_NeoPixel.h>
#define LED_PIN     23
#define LED_COUNT  6
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


const char* ssid       = "";
const char* password   = "";

const char* ntpServer = "ntp.jst.mfeed.ad.jp";
const long  gmtOffset_sec = 9*3600;
const int   daylightOffset_sec = 0;



#define SER   25
#define SRCLR 26
#define SRCLK 33
#define RCLK  32
#define DHTPIN 18

DHT dht(DHTPIN, DHT11);

void setup() {
  strip.begin();
  strip.show();
  strip.setBrightness(100);
  
  pinMode(SER,OUTPUT);
  pinMode(SRCLR,OUTPUT);
  pinMode(SRCLK,OUTPUT);
  pinMode(RCLK,OUTPUT);
   pinMode(4,INPUT);

  digitalWrite(SRCLR, LOW);
  digitalWrite(SRCLR, HIGH);

  Serial.begin(115200);
  rtc.begin();
  dht.begin();
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
WiFi.begin(ssid, password);
for(int i =0;i<6;++i){
strip.setPixelColor(i, strip.Color(192, 48, 192));//RGB
strip.show();
delay(500);
}
strip.fill(0,0,0);
strip.show();
delay(5000);
if (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("disconnect");
  strip.setPixelColor(0, strip.Color(255, 2, 0));//RGB
  strip.setPixelColor(1, strip.Color(255, 2, 0));
  strip.setPixelColor(2, strip.Color(255, 2, 0));
  strip.setPixelColor(3, strip.Color(255, 2, 0));
  strip.setPixelColor(4, strip.Color(255, 2, 0));
  strip.setPixelColor(5, strip.Color(255, 2, 0)); 
  strip.show(); 

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  }
 if(WiFi.status() == WL_CONNECTED){
  for(int i =0;i<6;++i){
  strip.setPixelColor(i, strip.Color(0, 0, 255));//RGB
  strip.show();
  delay(100);
}

  
  Serial.println(WiFi.status());


  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;
  getLocalTime(&timeinfo);


  byte Hour = timeinfo.tm_hour;
  Serial.print(Hour);
  byte Min  = timeinfo.tm_min;
  int Sec  = timeinfo.tm_sec;
    
  rtc.adjust(DateTime(2019,1,1,Hour,Min,Sec)); //時間以外は使わないので適当
 }
    

  //disconnect WiFi as it's no longer needed
//  WiFi.disconnect(true);
//  WiFi.mode(WIFI_OFF);
}
int pre(int x){
             //どこを光らせるか
           //A B C D E F G DP
    int one = B01100000; //1
    int two = B11011010; //2
    int thr = B11110010; //3
    int fou = B01100110; //4
    int fiv = B10110110; //5
    int six = B10111110; //6
    int sev = B11100000; //7
    int eit = B11111110; //8
    int nin = B11110110; //9
    int zer = B11111100; //0
    int err = B10011110; //errorE

switch(x){
  case 0:
  return zer;
  break;
  case 1:
  return one;
  break;
  case 2:
  return two;
  break;
  case 3:
  return thr;
  break;
  case 4:
  return fou;
  break;
  case 5:
  return fiv;
  break;
  case 6:
  return six;
  break;
  case 7:
  return sev;
  break;
  case 8:
  return eit;
  break;
  case 9:
  return nin;
  break;
  default:
  return err;
  break;
}   
}

void loop() {  
  strip.setPixelColor(0, strip.Color(48, 192, 187));
  strip.setPixelColor(1, strip.Color(48, 192, 187));
  strip.setPixelColor(2, strip.Color(48, 192, 187));
  strip.setPixelColor(3, strip.Color(48, 192, 187));
  strip.setPixelColor(4, strip.Color(48, 192, 187));
  strip.setPixelColor(5, strip.Color(48, 192, 187)); 
  strip.show(); 
    //初期化
    digitalWrite(SRCLR, LOW);  
    digitalWrite(SRCLR, HIGH);  
    
  DateTime now = rtc.now();
  int hour1 = now.hour() % 10; //1桁目
  int hour2 = (now.hour() - hour1)/10; //2桁目
  //Serial.print(10*hour2+hour1);
  //Serial.print(":");
  int min1 = now.minute() % 10; //1桁目
  int min2 = (now.minute() - min1)/10; //2桁目
  //Serial.print(10*min2+min1);
  //Serial.print(".");
  int sec1 = now.second() % 10; //1桁目
  int sec2 = (now.second() - sec1)/10; //2桁目
  //Serial.println(10*sec2+sec1);


    //シリパラ変換
    digitalWrite(RCLK, LOW);
    shiftOut(SER, SRCLK, LSBFIRST, pre(sec1));//右へ送る（こちらが一番右）
    shiftOut(SER, SRCLK, LSBFIRST, pre(sec2));
    shiftOut(SER, SRCLK, LSBFIRST, pre(min1));
    shiftOut(SER, SRCLK, LSBFIRST, pre(min2));
    shiftOut(SER, SRCLK, LSBFIRST, pre(hour1));
    shiftOut(SER ,SRCLK, LSBFIRST, pre(hour2));//先頭（左
    digitalWrite(RCLK, HIGH);

int fun = now.minute(); 
int byou = now.second();

if(hour1 == 1 && hour2 == 5 && fun==32){
  if(byou==50){
    WiFi.begin(ssid, password);
    for(int i =0;i<6;++i){
    strip.setPixelColor(i, strip.Color(192, 48, 192));//RGBW
    strip.show();
    delay(800);
}
if(WiFi.status() == WL_CONNECTED){
  strip.setPixelColor(0, strip.Color(0, 0, 255));//RGB
  strip.setPixelColor(1, strip.Color(0, 0, 255));
  strip.setPixelColor(2, strip.Color(0, 0, 255));
  strip.setPixelColor(3, strip.Color(0, 0, 255));
  strip.setPixelColor(4, strip.Color(0, 0, 255));
  strip.setPixelColor(5, strip.Color(0, 0, 255)); 
  strip.show(); 
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;
  getLocalTime(&timeinfo);


  byte Hour = timeinfo.tm_hour;
  Serial.print(Hour);
  byte Min  = timeinfo.tm_min;
  int Sec  = timeinfo.tm_sec;
    
  rtc.adjust(DateTime(2019,1,1,Hour,Min,Sec)); //時間以外は使わないので適当
  Serial.println("connect");
  delay(350);
}
if(WiFi.status() != WL_CONNECTED){
  strip.setPixelColor(0, strip.Color(255, 2, 0));//RGB
  strip.setPixelColor(1, strip.Color(255, 2, 0));
  strip.setPixelColor(2, strip.Color(255, 2, 0));
  strip.setPixelColor(3, strip.Color(255, 2, 0));
  strip.setPixelColor(4, strip.Color(255, 2, 0));
  strip.setPixelColor(5, strip.Color(255, 2, 0)); 
  strip.show(); 
}
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}
}
while(digitalRead(4) == LOW){
for(int i =0;i<6;++i){
strip.setPixelColor(i, strip.Color(random(1,255), random(1,255), random(1,255)));//RGBW
strip.show();
delay(100);
}
 int temp = dht.readTemperature()*100;
 int hum = dht.readHumidity();
 Serial.println(hum);
 int temp1 = temp%10;
 int temp2 = ((temp-temp1)/10)%10;
 int temp3 = (((temp-(temp2*10))-temp1)/100)%10;
 int temp4 = (temp/100-temp3)/10;
  
    digitalWrite(SRCLR, LOW);  
    digitalWrite(SRCLR, HIGH); 

    digitalWrite(RCLK, LOW);
    shiftOut(SER, SRCLK, LSBFIRST, pre(temp1)); //右へ送る（こちらが一番右）
    shiftOut(SER, SRCLK, LSBFIRST, pre(temp2));
    
    shiftOut(SER, SRCLK, LSBFIRST, pre(temp3));
    shiftOut(SER, SRCLK, LSBFIRST, pre(temp4)); //(左)
    digitalWrite(RCLK, HIGH);
       delay (10);
}
delay(100);
}
