#define BLYNK_TEMPLATE_ID "TMPL3asbf1X2b"
#define BLYNK_TEMPLATE_NAME "Weather Station"
#define BLYNK_AUTH_TOKEN "2MAjMdY2TDdh2BrynHuoMGE4rz3MJS12"

#include <DHT.h>  // HEADER FILES 
#include <ESP8266WiFi.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <Blynk.h>
#include <BlynkSimpleEsp8266.h>


#define DHTPIN 0
DHT dht(DHTPIN, DHT11);
Adafruit_BMP280 bmp;
#define SEALEVELPRESSURE_HPA (1013.25)
int sensorPin = A0;
int sensorValue2 = 0;  // variable to store the value coming from sensor Rain sensor
#define ALTITUDE 80.0 // Altitude of SparkFun's HQ in Boulder, CO. in meters
String apiKey = "IUTG165MGR8T65OL";//  Enter your Write API key from ThingSpeak
long myChannelNumber = 2336546;

const char *ssid = "Hey,GetYourOwnWi-Fi";
const char *pass = "12345678";
const char* server = "api.thingspeak.com";

WiFiClient client;


//--------------------------------------SET UP----------------------------------------------
void setup()
{
  Serial.begin(115200); // open serial port, set the baud rate to 9600 bps
  delay(10);
  dht.begin();
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED)
  {
  delay(200);
  Serial.print("..");
  }
  Serial.println();
  Serial.println("ESP8266 is connected!");
  Serial.println(WiFi.localIP());

  // Initialize the sensor (it is important to get calibration values stored on the device).

  if (bmp.begin(0x76))
  {
    Serial.println("BMP280 init success");
  }
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP280 init fail\n\n");
  }
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

}

//---------------------------------------LOOP FUNCTION-------------------------------------
void loop()
{
  Blynk.run();
//--------------------------------------DHT11--------------------------------
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      float dewPoint = dewPointFast(t, h);
      Serial.println("********************************");
      Serial.println("DHT11 sensor values:");
			Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" Degrees Celcius, Humidity: ");
      Serial.print(h);
      Serial.print(" %, DewPoint: ");
      Serial.print(dewPoint);
      Serial.print(" degrees Celsius ");
//--------------------------------------RAIN---------------------------------
  
      const int sensorMin = 150; //0;     // sensor minimum
      const int sensorMax = 440; //1024;  // sensor maximum
   
      delay(500);
      float sensorValue2;
      sensorValue2 = analogRead(sensorPin);
      sensorValue2 = constrain(sensorValue2, 150, 440); //150, 400
      sensorValue2 = map(sensorValue2, sensorMin, sensorMax, 0, 1023);  //150, 440
      Serial.println("********************************");
      Serial.println("Rain sensor values:");
      Serial.print("Rain value: ");
      Serial.print(sensorValue2);
      Serial.println();
      delay(100);
//---------------------------------BMP180-------------------------------
      float p,a;

      Serial.println("********************************");
      Serial.println("BMP280 sensor values:");

      p = bmp.readPressure() / 100.0F;
      Serial.print("Pressure: ");
      Serial.print(p,2);
      Serial.print("hPa, ");
         
      a = bmp.readAltitude(SEALEVELPRESSURE_HPA);
      Serial.print("Altitude: ");
      Serial.print(a,0);
      Serial.print(" meters, ");
      Serial.println();
//-------------------------------------------------ThingSpeak-------------------------------
              
      if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
           {  
                            
              String postStr = apiKey;
              postStr +="&field1=";
              postStr += String(t);//temperature
              postStr +="&field2=";
              postStr += String(h);//humidity
              postStr += "&field3=";
              postStr += String(dewPoint);//dew point
              postStr += "&field4=";
              postStr += String(p);//pressure
              postStr += "&field5=";
              postStr += String(a,0);//altitude
              postStr +="&field6=";
              postStr += String(sensorValue2);//rain
              postStr += "\r\n\r\n\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                            
                        }
      client.stop();
      Serial.print("Waiting...");
      Serial.println("");
      Serial.println("=========================================");
  
  // thingspeak needs minimum 15 sec delay between updates
  delay(5000);

  Blynk.virtualWrite(V0, t);  //V0 is for Temperature
  Blynk.virtualWrite(V1, h);  //V1 is for Humidity
  Blynk.virtualWrite(V2, dewPoint);  //V3 is for Dew Point
  Blynk.virtualWrite(V3, p);  //V4 is for Pressure
  Blynk.virtualWrite(V4, a);  //V5 is for Altitude
  Blynk.virtualWrite(V5, sensorValue2);  //V6 is for Rainfall
}

  double dewPointFast(double celsius, double humidity)
 {
   double a = 17.271;
   double b = 237.7;
   double temp = (a * celsius) / (b + celsius) + log(humidity * 0.01);
   double Td = (b * temp) / (a - temp);
   return Td;
 }
