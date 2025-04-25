#include "DHT.h"
#define DHT11_PIN 2

DHT dht11(DHT11_PIN, DHT11);

void setup() {
  Serial.begin(9600);
  dht11.begin(); // initialize the sensor
}

void loop() {
  // wait a few seconds between measurements.
  delay(2000);

  // read humidity
  float humi  = dht11.readHumidity();
  // read temperature as Celsius
  float tempC = dht11.readTemperature();


  // check if any reads failed
  if (isnan(humi) || isnan(tempC)) {
    Serial.println("Failed to read from DHT11 sensor!");
    if (isnan(humi)) {
      Serial.println("humi");
      }
    if (isnan(tempC)) {
      Serial.println("temp");
      }
  } else {
    Serial.print("DHT11# Humidity: ");
    Serial.print(humi);
    Serial.println("%");

    Serial.print("  |  "); 

    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.println("°C");

  }
}
