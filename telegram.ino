#include <ESP8266WiFi.h>
#include <DHT.h>
#include <UniversalTelegramBot.h>

const char* ssid= "Torpex";
const char* password = "wirelessattack";

const char* TOKEN = "7295761981:AAES27T0cAHlNPyzMaYG6yvQPgKJgDRez4M";
const char* CHAT_ID = "1546944812";

#define DHTTYPE DHT11
DHT dht1(2, DHTTYPE);
DHT dht2(0, DHTTYPE);

WiFiClientSecure client;
UniversalTelegramBot bot(TOKEN, client);

void setup() {
  Serial.begin(9600);
  dht1.begin();
  dht2.begin();
  WiFi.mode(WIFI_STA);
  client.setInsecure();
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
  if(old_h1 == -10000 || old_h2 == -10000 || old_t1 == -10000 || old_t2 == -10000){
    old_h1 = h1;
    old_h2 = h2;
    old_t1 = t1;
    old_t2 = t2;
    return;
  }
  if((abs(old_h1 - h1) > 10) || (abs(old_h2 - h2) > 10) || (abs(old_t1 - t1) > 0.7) || (abs(old_t2 - t2) > 0.7) || (abs(t2 - t1) > 4) || (abs(h2 - h1) > 40)) {
    if (bot.sendMessage(CHAT_ID, "Steam leak found", "")) {
      Serial.println("Alert sent to Telegram successfully.");
    }
  }
  old_h1 = h1;
  old_h2 = h2;
  old_t1 = t1;
  old_t2 = t2;

  delay(2000);
}
