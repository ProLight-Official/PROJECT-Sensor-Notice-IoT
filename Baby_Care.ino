  
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);

SoftwareSerial SoftWifi(8, 9);
#define SSID "Arduino"
#define PASS "RQFjoPt2"
#define IP "iot.quanta-camp.com"

void setup() {
  SoftWifi.begin(9600);
  Serial.begin(9600);
  lcd.init(); 
  lcd.backlight();
  lcd.print("Temperature:");
//Testing ESP8266
  SoftWifi.println("AT");
  delay(5000);
  if(SoftWifi.find("OK"))
  {
    Serial.println("RECEIVED: OK\nData Initializing!");
    SoftWifi.println("AT+CWMODE=1");
    delay(2000);
    //connect to wifi
    String cmd="AT+CWJAP=\"";
    cmd+=SSID;
    cmd+="\",\"";
    cmd+=PASS;
    cmd+="\"";
    SoftWifi.println(cmd);
    delay(3000);
    if(SoftWifi.find("OK"))
    {
      Serial.println("Connected");
    }
    else
    {
      Serial.println("Connection Failed");
    }
  }
  else
  {
    Serial.println("Did Not Receive: OK");
  }
}

void loop2()
{
    lcd.init();                      // initialize the lcd
  // Print a message to the LCD
  lcd.backlight();
  //lcd.print("Temperature:\n"); 
  //lcd.print(tempC);
  lcd.print("test");
  Serial.print("test");
  delay(5000);
}

void loop() {                       //loop starts
  
  int sensorvalue = analogRead(A0); //sound
    Serial.println(sensorvalue);
  delay(1000);
  
  int temperaturePin = 1; //temp sensor
  int reading = analogRead(temperaturePin);  
  float voltage = reading * 5.0;
  voltage /= 1024.0; 
  Serial.print(voltage); Serial.println(" volts");
  float tempC = (voltage - 0.5) * 100 ;  
  float tempF = (tempC * 9.0 / 5.0) + 32.0;
  Serial.println(tempC);
  delay(1000); 

                     // initialize the lcd
  // Print a message to the LCD
  lcd.setCursor(0, 1);
  lcd.print(tempC); lcd.print(" Celsius");
  
   String cmd = "AT+CIPSTART=\"TCP\",\"";
   cmd += IP;
   cmd += "\",80";
   SoftWifi.println(cmd);
   delay(5000);
   String receiveString="";
   while (SoftWifi.available())
   {
    char inChar = (char)SoftWifi.read();
    receiveString += inChar;
   }
   
  int length = 0;
  String monitorData = "";
  monitorData = monitorData + "{\"sensor1\":"+ sensorvalue +",\"sensor2\":"+ tempC +"}";   
  Serial.print(monitorData);
  
  length = monitorData.length();
  String data= "POST /v1/data/devices/bbcare/ HTTP/1.1\r\n";
  data += "Host: iot.quanta-camp.com\r\n";
  data += "Content-Type: application/json\r\n";
  data += "Accept:application/json\r\n";
  data += "Content-Length: "+ String(length) + "\r\n";
  data += "x-data-key: 0510ca4b33d64c38b1d8d888bc68dfc5\r\n";
  data += "\r\n";
  data += monitorData; 
    
  SoftWifi.print( "AT+CIPSEND=" );
  SoftWifi.println( data.length() );
  
  if(SoftWifi.find( ">" ) )
  {
    SoftWifi.println(data);
  }
  else
  {
    SoftWifi.println( "AT+CIPSEND=" );
  }
  
    if(SoftWifi.find("201"))
    { 
      Serial.println("Uploaded");
    }
    else {
      Serial.println("Upload failed");
    }
      delay(30000);
}    


