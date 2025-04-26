#include <ESP8266WiFi.h>
#include <DHT.h>
#include <UniversalTelegramBot.h>

//Wi-Fi сети и пароль для подключения
const char* ssid = "Torpex";
const char* password = "wirelessattack";

// Данные для подключения к Telegram Bot API
const char* TOKEN = "..." ; // Токен  нашего бота
const char* CHAT_ID = "..." ; // ID чата (пользователя или группы), куда отправлять уведомления

// Указываем тип датчиков DHT
#define DHTTYPE DHT11

// Инициализация двух датчиков DHT, подключённых к пинам 2 и 0 соответственно
DHT dht1(2, DHTTYPE);
DHT dht2(0, DHTTYPE);

// Создаем безопасный Wi-Fi клиент и экземпляр Telegram-бота
WiFiClientSecure client;
UniversalTelegramBot bot(TOKEN, client);

void setup() {
  Serial.begin(9600); 
  dht1.begin();
  dht2.begin();
  WiFi.mode(WIFI_STA); // Настройка работы модуля в режиме станции
  client.setInsecure(); // Отключение проверки SSL-сертификата (нужно для упрощённой работы HTTPS)
  
  // Ждем готовности Serial порта
  while (!Serial) {
    ;
  }
}

// Переменные для хранения предыдущих и текущих показаний датчиков
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
  // Проверяем наличие подключения к Wi-Fi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, password); // Пытаемся подключиться
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
  
  // Считываем показания температуры и влажности с обоих датчиков
  h1 = dht1.readHumidity();
  t1 = dht1.readTemperature();
  h2 = dht2.readHumidity();
  t2 = dht2.readTemperature();
  
  // Выводим их в Serial монитор для отладки
  Serial.printf("S1 → T: %.1f°C  H: %.1f%% | S2 → T: %.1f°C  H: %.1f%%\n", t1, h1, t2, h2);

  // Проверяем корректность чтения с первого датчика
  if (isnan(h1) || isnan(t1)) {
    Serial.println("Failed to read from DHT1 sensor!");
    delay(2000);
    return
  }

  // Проверяем корректность чтения со второго датчика
  if (isnan(h2) || isnan(t2)) {
    Serial.println("Failed to read from DHT2 sensor!");
    delay(2000);
    return;
  }

  // Проверка первая ли это итеррация
  if(old_h1 == -10000 || old_h2 == -10000 || old_t1 == -10000 || old_t2 == -10000){
    old_h1 = h1;
    old_h2 = h2;
    old_t1 = t1;
    old_t2 = t2;
    return;
  }

  // Условия для обнаружения утечки:
  if((abs(old_h1 - h1) > 10) || (abs(old_h2 - h2) > 10) || (abs(old_t1 - t1) > 0.7) || (abs(old_t2 - t2) > 0.7) || (abs(t2 - t1) > 4) || (abs(h2 - h1) > 40)) {
    // Отправляем сообщение в Telegram
    if (bot.sendMessage(CHAT_ID, "Steam leak found", "")) {
      Serial.println("Alert sent to Telegram successfully.");
    }
  }

  // Обновляем старые данные
  old_h1 = h1;
  old_h2 = h2;
  old_t1 = t1;
  old_t2 = t2;

  delay(2000);
}
