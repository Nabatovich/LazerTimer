#include <ESP8266WiFi.h>

#define PIN_PHOTO_SENSOR A0

const char *ssid = "LazerTimer";  // Название Wi-Fi сети первой платы
const char *password = "11559900";  // Пароль Wi-Fi сети первой платы
const char *serverIP = "192.168.4.1";  // IP адрес первой платы

WiFiClient client;

bool flag = false;

void setup() {
  Serial.begin(9600);
  delay(10);

  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);

  // Подключение к Wi-Fi сети
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {  // Ожидание подключения
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  // Выводим IP адрес

  client.connect(serverIP, 80);  // Подключаемся к серверу на первой плате
  delay(9000);

  WiFi.persistent(true);
}

void loop() {
  if (client.connected()) {  // Если подключены к серверу
    //Serial.println("[1]");

    int fot = analogRead(PIN_PHOTO_SENSOR);
    Serial.println(fot);

    digitalWrite(D2, HIGH);
    if (fot > 800) {
      digitalWrite(D3, HIGH);
      sendCommand(flag ? "start\r" : "finish\r");
    } else {
      digitalWrite(D3, LOW);
    }
  } else {  // Если потеряно соединение
    Serial.println("Connection lost");
    client.connect(serverIP, 80);  // Переподключаемся
    digitalWrite(D2, LOW);
    delay(5000);
  }
}

void sendCommand(const char* command) {
  client.print(command);  // Отправляем данные на сервер
  Serial.print(command);
  delay(500);
}
