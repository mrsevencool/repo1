#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

const char* ssid= "Torpex";
const char* password = "wirelessattack";

int channelID = 2936178;
const char* writeAPIKey = "6TP6Y8RRP6V21DIO";

#define DHTTYPE DHT11
DHT dht1(2, DHTTYPE);
DHT dht2(0, DHTTYPE);

WiFiClient client;

void setup() {
  Serial.begin(9600);
  ThingSpeak.begin(client);
  dht1.begin();
  dht2.begin();
  WiFi.mode(WIFI_STA);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
}

float old_h1 = -10000;
float old_h2 = -10000;
float old_t1 = -10000;
float old_t2 = -10000;
float h1 = -10000;
float h2 = -10000;
float t1 = -10000;
float t2 = -10000;
int counter = 0;

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, password);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  
  h1 = dht1.readHumidity();
  t1 = dht1.readTemperature();
  h2 = dht2.readHumidity();
  t2 = dht2.readTemperature();
  Serial.printf("S1 → T: %.1f°C  H: %.1f%% | S2 → T: %.1f°C  H: %.1f%%\n", t1, h1, t2, h2);
  if (isnan(h1) || isnan(t1)) {
    Serial.println("Failed to read from DHT1 sensor!");
    delay(2000);
    return;
  }
  if (isnan(h2) || isnan(t2)) {
    Serial.println("Failed to read from DHT2 sensor!");
    delay(2000);
    return;
  }
  if(old_h1 == -10000 || old_h2 == -10000 || old_t1 == -10000 || old_t2 == -10000) {
    old_h1 = h1;
    old_h2 = h2;
    old_t1 = t1;
    old_t2 = t2;
    return;
  }
  if(counter == 10){
    counter = 0;
    ThingSpeak.setField(1, t1);
    ThingSpeak.setField(2, h1);
    ThingSpeak.setField(3, t2);
    ThingSpeak.setField(4, h2);
    ThingSpeak.writeFields(channelID, writeAPIKey);
    Serial.println("Sent to ThingSpeak!");
  } else {
    counter += 1;
  }
  old_h1 = h1;
  old_h2 = h2;
  old_t1 = t1;
  old_t2 = t2;

  delay(2000);
}
