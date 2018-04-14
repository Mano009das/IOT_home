#include <DHT.h>
#include <DHT_U.h>

//backup

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include <Servo.h>
//#define DHTPIN            13
#define WLAN_SSID       "TP-LINK"
#define WLAN_PASS       "1qazxsw2"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  8883                  
#define AIO_USERNAME    "mano009das"
#define AIO_KEY         "2450f209b3044bb98064a237cea0c171"
WiFiClientSecure client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish Temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temp", MQTT_QOS_1);
Adafruit_MQTT_Publish Humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity", MQTT_QOS_1);


Adafruit_MQTT_Subscribe Everything = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Everything", MQTT_QOS_1);
Adafruit_MQTT_Subscribe Light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Light", MQTT_QOS_1);
Adafruit_MQTT_Subscribe Radio = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Radio", MQTT_QOS_1);
Adafruit_MQTT_Subscribe Bulb = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Bulb", MQTT_QOS_1);
Adafruit_MQTT_Subscribe Fan = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Fan", MQTT_QOS_1);


#define DHTTYPE           DHT11  
DHT dht(13, DHTTYPE);
uint32_t delayMS;

void MQTT_connect();

Servo myservo;
void setup() {


  
  Serial.begin(115200);
  delay(10);
  myservo.attach(16);
  pinMode(14,OUTPUT);
  pinMode(12,OUTPUT);
 // pinMode(16,OUTPUT);
  pinMode(5,OUTPUT);
  Serial.println(F("HOME AUTOMATION USING MQTT AND GOOGLE ASSISTANCE VOICE CONTROL"));
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  mqtt.subscribe(&Light);
  mqtt.subscribe(&Radio);
  mqtt.subscribe(&Bulb);
  mqtt.subscribe(&Fan);
  mqtt.subscribe(&Everything);

}

  

uint32_t x=0;

void loop() {

  MQTT_connect();


  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(15000))) {
  
  if (subscription == &Everything) {
      Serial.print(F("Got_Everything: "));
      Serial.println((char *)Everything.lastread);
      uint16_t num = atoi((char *)Everything.lastread);
       digitalWrite(14,num);
    //   digitalWrite(12,num);
     //  digitalWrite(16,num);
       digitalWrite(5,num);
  }
  if (subscription == &Light) {
      Serial.print(F("Got_light: "));
      Serial.println((char *)Light.lastread);
      uint16_t num = atoi((char *)Light.lastread);
      digitalWrite(14,num);
    }
     if (subscription == &Radio) {
      Serial.print(F("Got_auto: "));
      Serial.println((char *)Radio.lastread);
      uint16_t num = atoi((char *)Radio.lastread);
      digitalWrite(12,num);
    }
     if (subscription == &Bulb) {
      Serial.print(F("Got_servo: "));
      Serial.println((char *)Bulb.lastread);
      int  servo_State = atoi((char *)Bulb.lastread);
       myservo.write(servo_State);
      //digitalWrite(16,num);
    }
     if (subscription == &Fan) {
      Serial.print(F("Got_Fan: "));
      Serial.println((char *)Fan.lastread);
      uint16_t num = atoi((char *)Fan.lastread);
      digitalWrite(5,num);
    }

  }
   // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  {if ( atoi((char *)Radio.lastread) == 1)
{
   float t = dht.readTemperature();
   Serial.print("temperature = ");
    Serial.print(t); 
    Serial.println("C  ");
    
    if (t>=29)
   digitalWrite(5,HIGH);
   if (t<29)
   digitalWrite(5,LOW);
   else if 
   ( atoi((char *)Radio.lastread) == 0)
   {digitalWrite(5,LOW);}
   }
   
}
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
    
  }
  if (! Humidity.publish(h)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  if (! Temp.publish(t)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

}
void MQTT_connect() {
  int8_t ret;
  
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { 
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(10000);
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
