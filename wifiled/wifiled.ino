#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"
#include "RTClib.h"
#include <Arduino.h>
#include "esp_wps.h"
RTC_DS3231 rtc;
WiFiServer server(80);
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



#define ESP_WPS_MODE      WPS_TYPE_PBC
#define ESP_MANUFACTURER  "ESPRESSIF"
#define ESP_MODEL_NUMBER  "ESP32"
#define ESP_MODEL_NAME    "ESPRESSIF IOT"
#define ESP_DEVICE_NAME   "ESP STATION"

static esp_wps_config_t config;



#define LED_WHITE A4

//const char *ssid = "yourAP";
const char *password = "yourPassword";
//const IPAddress ip(192, 168, 6, 1);
//const IPAddress subnet(255, 255, 255, 0);



// HTTPリクエストを格納する変数
String header;

// 値の設定に使用する変数
String valueString = String(5);
int Rval = 0;
int Bval = 0;
int Wval = 0;
int UVval = 0;
static int Mode=1;//1=off,2=0n,3=timer
int pos1 = 0;
int pos2 = 0;
  static int starthour = 6;
  static int startminute =30;
  static int stophour = 18;
  static int stopminute =30;
  
void setup() {


  rtc.begin();

  
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
 // Pause for 2 seconds
display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 0);

      DateTime now = rtc.now();

    display.print(now.year(), DEC);
    display.print('/');
    display.print(now.month(), DEC);
    display.print('/');
    display.println(now.day(), DEC);
    display.print(now.hour(), DEC);
    display.print(':');
    display.print(now.minute(), DEC);
    display.print(':');
    display.print(now.second(), DEC);
    display.println();
    display.display();
  delay(2000);
  //GR
  ledcSetup(0, 12800, 8);
  // ledGRをチャネル0へ接続
  ledcAttachPin(A4, 0);

  //W
  ledcSetup(1, 12800, 8);
  // whiteをチャネル1へ接続
  ledcAttachPin(A5, 1);

  //B
  ledcSetup(2, 12800, 8);
  // blueをチャネル2へ接続
  ledcAttachPin(A18, 2);
  
    //uv
  ledcSetup(3, 12800, 8);
  // uvをチャネル3へ接続
  ledcAttachPin(A19, 3);

  Serial.begin(115200);
  Serial.println();
  display.println("Configuring access point...");
  display.display();

/*
  // パスワードは無しにすることもできます。
  WiFi.softAP(ssid, password);
//    WiFi.softAPConfig(ip, ip, subnet);
  IPAddress myIP = WiFi.softAPIP();
  display.print("AP IP address: ");
  display.println(myIP);
  server.begin();
    display.println("Server started");
  display.display();
  */

}

bool timenon(){
              DateTime now = rtc.now();
    if(now.hour()>stophour){  //２時半すぎた？
      return true;   //過ぎた
        }else{                    //１時程度か、２時何分か
          if(now.hour()<stophour){ //まだ１時とかで低い感じ
            return false;
            }else if(now.minute()>stopminute){   //２時の何分か（分単位は過ぎたか？）
             return true;           //30分回ってた
           }else if(now.minute()==stopminute){     //ぴったりだった、止めよう
            return false;
           }else if(now.minute()<stopminute){    //まだ30分じゃない
            return false;
           }
      }
    }

bool timeact(){
            DateTime now = rtc.now();
    if(now.hour()<starthour){  //1時半なった？
      return false;   //まだ
        }else{                    //0時程度か、1時何分か
          if(now.hour()>starthour){ //2時回ってる
            return true;
            }else if(now.minute()>startminute){   //1時の何分か（分単位は過ぎたか？）
             return true;           //30分回ってた
           }else if(now.minute()==startminute){     //ぴったりだった、つけよう
            return true;
           }else if(now.minute()<startminute){    //まだ30分じゃない
            return false;
           }
      }
    }

    int rtnmonth(String m){

        if(m == "Jan"){
        return 1;
        }

        if(m == "Feb"){
        return 2;
        }

        if(m == "Mar"){
        return 3;
        }

        if(m == "Apr"){
        return 4;
        }

        if(m == "May"){
        return 5;
        }

        if(m == "Jun"){
        return 6;
        }

        if(m == "Jul"){
        return 7;
        }

        if(m == "Aug"){
        return 8;
        }

        if(m == "Sep"){
        return 9;
        }

        if(m == "Oct"){
        return 10;
        }

        if(m == "Nov"){
        return 11;
        }

        if(m == "Dec"){
        return 12;
        }
      }
void wpsInitConfig(){
  config.crypto_funcs = &g_wifi_default_wps_crypto_funcs;
  config.wps_type = ESP_WPS_MODE;
  strcpy(config.factory_info.manufacturer, ESP_MANUFACTURER);
  strcpy(config.factory_info.model_number, ESP_MODEL_NUMBER);
  strcpy(config.factory_info.model_name, ESP_MODEL_NAME);
  strcpy(config.factory_info.device_name, ESP_DEVICE_NAME);
}

String wpspin2string(uint8_t a[]){
  char wps_pin[9];
  for(int i=0;i<8;i++){
    wps_pin[i] = a[i];
  }
  wps_pin[8] = '\0';
  return (String)wps_pin;
}

void WiFiEvent(WiFiEvent_t event, system_event_info_t info){
  switch(event){
    case SYSTEM_EVENT_STA_START:
      Serial.println("Station Mode Started");
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.print("Connected to :");
      Serial.println(String(WiFi.SSID()));
      Serial.print("Got IP: ");
      Serial.println(WiFi.localIP());
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("Disconnected from station, attempting reconnection");
      WiFi.reconnect();
      break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
      Serial.println("WPS Successfull, stopping WPS and connecting to: " + String(WiFi.SSID()));
      esp_wifi_wps_disable();
      delay(10);
      WiFi.begin();
      server.begin();
      break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
      Serial.println("WPS Failed, retrying");
      esp_wifi_wps_disable();
      esp_wifi_wps_enable(&config);
      esp_wifi_wps_start(0);
      break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
      Serial.println("WPS Timedout, retrying");
      esp_wifi_wps_disable();
      esp_wifi_wps_enable(&config);
      esp_wifi_wps_start(0);
      break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
      Serial.println("WPS_PIN = " + wpspin2string(info.sta_er_pin.pin_code));
      break;
    default:
      break;
  }
}
void loop() {
if(digitalRead(4)==LOW){
  delay(10);
    WiFi.onEvent(WiFiEvent);
  WiFi.mode(WIFI_MODE_STA);
  
       display.clearDisplay();
    display.setCursor(1, 0);
    display.setTextSize(1);
  display.println("Starting WPS");
  display.display();

  wpsInitConfig();
  esp_wifi_wps_enable(&config);
  esp_wifi_wps_start(0);
  delay(5000);
    }



  
   WiFiClient client = server.available();   // Listen for incoming clients
       display.clearDisplay();
    display.setCursor(1, 0);
          DateTime now = rtc.now();
    display.print(now.year(), DEC);
    display.print('/');
    display.print(now.month(), DEC);
    display.print('/');
    display.println(now.day(), DEC);
    display.setTextSize(2);
    display.print(now.hour(), DEC);
    display.print(':');
    display.print(now.minute(), DEC);
    display.print(':');
    display.print(now.second(), DEC);
    display.println();
    display.println();
    display.setTextSize(1);
    display.print("start->");
    display.print(starthour);
    display.print(":");
    display.println(startminute);
    display.print("stop->");
    display.print(stophour);
    display.print(":");
    display.println(stopminute);
    
      display.println(WiFi.localIP());
    display.display();
      
   
   switch(Mode){
    case 1:
                ledcWrite(0,0);
                ledcWrite(1,0);
                ledcWrite(2,0);
                ledcWrite(3,0);
                break;
    case 2:

                ledcWrite(0,Rval);
                ledcWrite(1,Wval);
                ledcWrite(2,Bval);
                ledcWrite(3,UVval);
                break;

    
    case 3:

if(timeact()==true){  //点く時間なったか？
  if(timenon()==true){   //もう時間過ぎたか？->はい
                ledcWrite(0,0);
                ledcWrite(1,0);
                ledcWrite(2,0);
                ledcWrite(3,0);
  }else{
                ledcWrite(0,Rval);
                ledcWrite(1,Wval);
                ledcWrite(2,Bval);
                ledcWrite(3,UVval);    
  }
          
}else{
                ledcWrite(0,0);
                ledcWrite(1,0);
                ledcWrite(2,0);
                ledcWrite(3,0);
}
                break;
   }

  if (client) {                             // If a new client connects,
    display.clearDisplay();
      display.setCursor(10, 0);
    display.println("New Client."); 
    display.display();                      // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>ESP32 RGB LED controller</title>");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
            client.println("#servoPosR{color: red;}");
            client.println("#servoPosW{color: yellow;}");
            client.println("#servoPosB{color: blue;}");
            client.println("#servoPosU{color: purple;}");
    client.println("#startH{color : aqua;}");
    client.println("#startM{color : aqua;}");
    client.println("#stopH{color : blueviolet;}#stopM{color : blueviolet;}");
    client.println(".form {width: 40px;}");
            client.println(".slider { width: 300px; }</style>");
            
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
                     

                     
            // Web Page
            client.println("</head><body><h1>LED-Coral-Light</h1>");
            client.println("<p>GlobalTIME: <span id=\"timef\"></span></p>");
            client.println("<p>localTime:\""+String(now.year())+"."+String(now.month())+"/"+String(now.day())+"-"+String(now.hour())+":"+String(now.minute())+"."+String(now.second())+"\"</p>");
            client.println("<input type=\"button\" value=\"time_adjustment_button\" onclick=\"timesetf(),timeset(timeview)\" name=\"tsb\">");
            //bottuns
            client.println("<br><br><input type=\"button\" value=\"ON\" onclick=\"ONfunc(),send(2)\" name=\"ONB\">");
    
            client.println("<input type=\"button\" value=\"OFF\" onclick=\"OFFfunc(),send(1)\" name=\"OFFB\">");

            client.println("<input type=\"button\" value=\"TIMER\" onclick=\"TIMERfunc(),send(3)\" name=\"OFFB\">");
            client.println("<p>Mode: <span id=\"statnameP\"></span></p>");
            //GR slide bar
            client.println("<p>Brightness of Red: <span id=\"servoPosR\"></span></p>");          
            client.println("<input type=\"range\" min=\"0\" max=\"255\" class=\"slider\" id=\"servoSliderR\" onchange=\"servo(this.value,'Red')\" value=\""+valueString+"\"/>");
            //white slide bar
            client.println("<p>Brightness of White: <span id=\"servoPosW\"></span></p>");          
            client.println("<input type=\"range\" min=\"0\" max=\"255\" class=\"slider\" id=\"servoSliderW\" onchange=\"servo(this.value,'White')\" value=\""+valueString+"\"/>");
            //blue slide bar
            client.println("<p>Brightness of Blue: <span id=\"servoPosB\"></span></p>");          
            client.println("<input type=\"range\" min=\"0\" max=\"255\" class=\"slider\" id=\"servoSliderB\" onchange=\"servo(this.value,'Blue')\" value=\""+valueString+"\"/>");
            //uv
            client.println("<p>Brightness of UV: <span id=\"servoPosU\"></span></p>");          
            client.println("<input type=\"range\" min=\"0\" max=\"255\" class=\"slider\" id=\"servoSliderU\" onchange=\"servo(this.value,'UV')\" value=\""+valueString+"\"/>");
            //Day
    client.println("<br><h1>timer</h1>");
    client.println("<p>start(24H)-> <span id=\"startH\"></span>:<span id=\"startM\"></span></p>");
    client.println("<p>stop(24H)-> <span id=\"stopH\"></span>:<span id=\"stopM\"></span></p>");
    
    client.println("<br><br><p>StartSettings</p>");
    client.println("hour:<input type=\"number\" step=\"1\" min=\"0\" class=\"form\" max=\"23\" id=\"thour\" value=\""+String(starthour)+"\" required onchange=\"servoT(this.value,'sth')\"> minute:<input type=\"number\" step=\"1\" min=\"0\" class=\"form\" max=\"59\" id=\"tminute\" value=\""+String(startminute)+"\" required  onchange=\"servoT(this.value,'stm')\">");
    client.println("<br><br><p>StopSettings</p>");
    client.println("hour:<input type=\"number\" step=\"1\" min=\"0\" class=\"form\" max=\"23\" id=\"shour\" value=\""+String(stophour)+"\" required onchange=\"servoT(this.value,'sph')\"> minute:<input type=\"number\" step=\"1\" min=\"0\" class=\"form\" max=\"59\" id=\"sminute\" value=\""+String(stopminute)+"\" required onchange=\"servoT(this.value,'spm')\">");


            client.println("<script>");
            //send R value
            client.println("var sliderR = document.getElementById(\"servoSliderR\");");
            client.println("var servoPR = document.getElementById(\"servoPosR\"); servoPR.innerHTML = sliderR.value;");
            client.println("sliderR.oninput = function() { sliderR.value = this.value; servoPR.innerHTML = this.value; }");

            //send G value
            client.println("var sliderW = document.getElementById(\"servoSliderW\");");
            client.println("var servoPW = document.getElementById(\"servoPosW\"); servoPW.innerHTML = sliderW.value;");
            client.println("sliderW.oninput = function() { sliderW.value = this.value; servoPW.innerHTML = this.value; }");
            //send B value
            client.println("var sliderB = document.getElementById(\"servoSliderB\");");
            client.println("var servoPB = document.getElementById(\"servoPosB\"); servoPB.innerHTML = sliderB.value;");
            client.println("sliderB.oninput = function() { sliderB.value = this.value; servoPB.innerHTML = this.value; }");
            //send UV value
            client.println("var sliderU = document.getElementById(\"servoSliderU\");");
            client.println("var servoPU = document.getElementById(\"servoPosU\"); servoPU.innerHTML = sliderU.value;");
            client.println("sliderU.oninput = function() { sliderU.value = this.value; servoPU.innerHTML = this.value; }");

            //HTTP getのための関数
            client.println("$.ajaxSetup({timeout:1000}); function servo(pos,color) { ");
            client.println("$.get(\"/?value\" + color + \"=\" + pos + \"&\"); {Connection: close};}</script>");
           
            client.println("</body></html>"); 

    client.println("<script>"); 
    client.println("var Hour = document.getElementById(\"thour\");"); 
    client.println("var timeHour = document.getElementById(\"startH\"); timeHour.innerHTML = Hour.value;"); 
    client.println("Hour.oninput = function() { Hour.value = this.value; timeHour.innerHTML = this.value; }"); 
    
    client.println("var Minute = document.getElementById(\"tminute\");"); 
    client.println("var timeMinute = document.getElementById(\"startM\"); timeMinute.innerHTML = Minute.value;"); 
    client.println("Minute.oninput = function() { Minute.value = this.value; timeMinute.innerHTML = this.value; }"); 
    
    client.println("var SHour = document.getElementById(\"shour\");"); 
    client.println("var StimeHour = document.getElementById(\"stopH\"); StimeHour.innerHTML = SHour.value;"); 
    client.println("SHour.oninput = function() { SHour.value = this.value; StimeHour.innerHTML = this.value; }"); 
    
    client.println("var SMinute = document.getElementById(\"sminute\");"); 
    client.println("var StimeMinute = document.getElementById(\"stopM\"); StimeMinute.innerHTML = SMinute.value;"); 
    client.println("SMinute.oninput = function() { SMinute.value = this.value; StimeMinute.innerHTML = this.value; }"); 
    
    client.println("$.ajaxSetup({timeout:1000}); function servoT(tim,status) { "); 
    client.println("$.get(\"/?value\" + status + \"=\" + tim + \"&\"); {Connection: close};}"); 
    
    client.println("</script>"); 

  client.println("<script>"); 
  client.println("var stat;  "); 
  client.println("var statname = \"select plz\";"); 
  client.println("var statnameS = document.getElementById(\"statnameP\"); statnameS.innerHTML = statname;"); 
  client.println("var ONfunc = function(){stat = 1;statname = \"ON\";statnameS.innerHTML = statname;alert(statname);}"); 
  client.println("var OFFfunc = function(){stat = 2;statname = \"OFF\";statnameS.innerHTML = statname;alert(statname);}"); 
  client.println("var TIMERfunc = function(){stat = 3;statname = \"TIMER-MODE\";statnameS.innerHTML = statname;alert(statname);}"); 
  client.println("$.ajaxSetup({timeout:1000}); function send(status) {"); 
        
  client.println("$.get(\"/?status\" + \":\" + status+ \"&\"); {Connection: close};}"); 
  client.println("</script>");


    client.println("<script>");
    
    client.println("var timeview=\"plz push the time_adjustment_button\";document.getElementById(\"timef\"); timef.innerHTML=timeview;");
    client.println("var timesetf = function(){timeview = new Date(); alert(timeview);document.getElementById(\"timef\"); timef.innerHTML=timeview;}");
    client.println("$.ajaxSetup({timeout:1000}); function timeset(tmv) {");
    client.println("$.get(\"/?timesetfunc\" + \"=\" + tmv +\"&\"); {Connection: close};}");
 
    client.println("</script>");

  

            //HTTPリクエストの処理部分
            //GET /?value=180& HTTP/1.1

              //時刻合わせ
              if(header.indexOf("GET /?timesetfunc")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf("GMT");
              valueString = header.substring(pos2-11,pos2-9);
              int hour = valueString.toInt();
              Serial.println(valueString);
              valueString = header.substring(pos2-8, pos2-6);
              int minute = valueString.toInt();
              valueString = header.substring(pos2-5, pos2-3);
              int second = valueString.toInt();
              valueString = header.substring(pos1+7, pos1+10);
              int tmonth = rtnmonth(valueString);
              valueString = header.substring(pos1+13, pos1+15);
              int tday = valueString.toInt();
              valueString = header.substring(pos1+18, pos1+22);
              int tyear = valueString.toInt();
              Serial.println(valueString);
              
              
              //取り出して表示
              Serial.print("hour=");
              Serial.println(hour); 
              Serial.print("minute=");
              Serial.println(minute);
              Serial.print("second=");
              Serial.println(second);
              Serial.print("year=");
              Serial.println(tyear);
              Serial.print("month=");
              Serial.println(tmonth);
              Serial.print("day=");
              Serial.println(tday);
              rtc.adjust(DateTime(tyear,tmonth,tday,hour, minute, second));                  
            }  

            //btnの値を状態に変換
            if(header.indexOf("GET /?status:")>=0) {
              pos1 = header.indexOf(':');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              
              //点灯か消灯かタイマーか
              Mode = valueString.toInt();
              Serial.println(Mode); 
            }  
            
            //Rの値をledの出力に変換
            if(header.indexOf("GET /?valueRed=")>=0){
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              Rval = valueString.toInt();
              
              //valueStringの値を表示
                  display.clearDisplay();
                  display.setCursor(10, 0);
              display.println(valueString); 
            }  
            //Wの値をledの出力に変換
            if(header.indexOf("GET /?valueWhite=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              Wval = valueString.toInt();
              
              //LEDをvalueStringの値で点灯
                  display.clearDisplay();
                display.setCursor(10, 0);
              display.println(valueString); 
            }  

            //Bの値をledの出力に変換
            if(header.indexOf("GET /?valueBlue=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              Bval = valueString.toInt();
              
              //LEDをvalueStringの値で点灯
                  display.clearDisplay();
                display.setCursor(10, 0);
              display.println(valueString); 
            }

              //UVの値をledの出力に変換
              if(header.indexOf("GET /?valueUV=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              UVval = valueString.toInt();
              
              //LEDをvalueStringの値で点灯
                  display.clearDisplay();
                display.setCursor(10, 0);
              display.println(valueString);     
            }


            
              //timeの値を数字に変換
              if(header.indexOf("GET /?valuesth=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              starthour = valueString.toInt();
              
              //LEDをvalueStringの値で点灯
                  display.clearDisplay();
                display.setCursor(10, 0);
              display.print("startH:");
              display.println(starthour);  
              }

              //timeの値を数字に変換
              if(header.indexOf("GET /?valuestm=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              startminute = valueString.toInt();
              
              //LEDをvalueStringの値で点灯
                  display.clearDisplay();
                display.setCursor(10, 0);
              display.print("startM:");
              display.println(startminute);  
              }

              //timeの値を数字に変換
              if(header.indexOf("GET /?valuesph=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              stophour = valueString.toInt();
              
              //LEDをvalueStringの値で点灯
                  display.clearDisplay();
                display.setCursor(10, 0);
              display.print("stopH:");
              display.println(stophour);  
              }

              //timeの値を数字に変換
              if(header.indexOf("GET /?valuespm")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              stopminute = valueString.toInt();
              
              //LEDをvalueStringの値で点灯
                  display.clearDisplay();
                display.setCursor(10, 0);
              display.print("stopM:");
              display.println(stopminute);  
              }
 
           



            
            // HTTPレスポンスの終了
            client.println();
            // Break out of the while loop
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    // Clear the header variable
    header = "";
    // 接続を切断
    client.stop();
    display.println("Client disconnected.");
    display.println("");
              display.display();   
  }
          
    }
