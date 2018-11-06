/* mostly copy/paste from Rui Santos http://randomnerdtutorials.com */

#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>

const char* ssid = "yourssid";
const char* password = "";

const char* ssid2 = "yoursecondssid";
const char* password2 = "";

const char* mqtt_server = "37.187.106.16"; //test.mosquitto.org

char msg[50];
long waited = 0;

// Data wire is plugged into pin D1 on the ESP8266 12-E - GPIO 5
#define ONE_WIRE_BUS D1

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature DS18B20(&oneWire);
char temperatureCString[7];

WiFiClient espClient;
PubSubClient client(espClient);

// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);
  DS18B20.begin(); 
  WiFi.mode(WIFI_STA);
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  waitForWifi();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(ssid2);
    WiFi.begin(ssid2, password2);
    waitForWifi();
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Going DeepSleep");
    Serial.println("no wifi, sleep 3 hrs");
    ESP.deepSleep(3e10); 
  }
    
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
}

void reconnect() {
  // Loop until we're reconnected
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    if (client.connect("ESP8266Client")) {
    //if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print(".. give up, no connectivity. Sleep 3 hrs min");
      ESP.deepSleep(3e10); 
    }
}

void getTemperature() {
  float tempC;
  float tempF;
  do {
    DS18B20.requestTemperatures(); 
    tempC = DS18B20.getTempCByIndex(0);
    dtostrf(tempC, 2, 2, temperatureCString);
    delay(100);
  } while (tempC == 85.0 || tempC == (-127.0));

}

void waitForWifi(){
  waited = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    waited = waited + 500;
    if(waited > 20000){
      break;
    }
  }
}

void loop() {
  client.loop();
  getTemperature();
  Serial.println(temperatureCString);

  if (!client.connected()) {
    reconnect();
  }
  client.publish("norway/hovik/sea/temperature" , temperatureCString, true);    
  ESP.deepSleep(3e10); 
  
}


