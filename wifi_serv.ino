#include <ESP8266WiFi.h>
#include "GyverTM1637.h"

const char *ssid = "LazerTimer";  // Название Wi-Fi сети
const char *password = "11559900";  // Пароль Wi-Fi сети

WiFiServer server(80);  // Создаем сервер на порту 80
GyverTM1637 display(D7, D5);

unsigned long startTime = 0;    // Время начала отсчета
unsigned long lastTime = 0;     // Прошедшее время

bool measuringTime = false;  // Флаг измерения времени

const int maxClients = 5; // Максимальное количество подключенных клиентов
WiFiClient clients[maxClients]; // Массив для клиентских объектов

void setup() {
  Serial.begin(9600);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);

  display.brightness(7);  // Яркость, 0 - 7 (минимум - максимум)
  
  byte welcome_banner[] = {_H, _E, _L, _L, _O, _empty, _empty,
                           _G, _O, _O, _D, _empty, _empty,
                           _D, _A, _Y, _empty
                          };
  display.runningString(welcome_banner, sizeof(welcome_banner), 300);  // выводим
  display.displayByte(_c, _o, _o, _n);

  delay(500);

  // Подключение к Wi-Fi сети
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.softAP(ssid, password);  // Создание Wi-Fi точки доступа

  IPAddress ip = WiFi.softAPIP();  // Получаем IP адрес точки доступа
  Serial.print("AP IP address: ");
  Serial.println(ip);

  server.begin();  // Запускаем сервер
  //display.displayByte(0x40, 0x40, 0x40, 0x40);
}

void loop() {
  // Проверяем наличие новых подключений
  WiFiClient newClient = server.available();
  if (newClient) {
    Serial.println("New client");
    // Ищем свободное место в массиве клиентов
    for (int i = 0; i < maxClients; i++) {
      if (!clients[i] || !clients[i].connected()) {
        clients[i] = newClient;
        if (i == 0) {
        display.displayByte(_1, _empty, _o, _n);
        }else if (i == 2) {
        display.displayByte(_2, _empty, _o, _n);
        }
        break;
      }
    }
  }

  // Обрабатываем каждого клиента
  for (int i = 0; i < maxClients; i++) {
    if (clients[i].connected()) {
      handleClient(clients[i]);
    }
    manageDigitalOutputs(i, clients[i].connected());
  }
}

void handleClient(WiFiClient client) {
  timer();
  
  if (client.available()) {
    String command = client.readStringUntil('\r');  // Читаем команду
    if (command == "start" && !measuringTime) {
      startTime = millis();
      measuringTime = true;
      Serial.println("[T] Timer start");
    } else if (command == "finish" && measuringTime) {
      measuringTime = false;
      Serial.println("[T] Timer finish");
    }
  }
}

void timer() {
  if (measuringTime) {
    lastTime = millis() - startTime;
    displayTime(lastTime);
  }
}

void displayTime(unsigned long time) {
  int seconds = time / 1000;       // Переводим миллисекунды в секунды
  int milliseconds = time % 1000;  // Оставшиеся миллисекунды

  // Выводим минуты и секунды на дисплей
  display.point(true);
  display.display(0, (seconds / 10) % 10);
  display.display(1, seconds % 10);
  display.display(2, (milliseconds / 100) % 10);
  display.display(3, (milliseconds / 10) % 10);
}

void manageDigitalOutputs(int index, bool isConnected) {
  if (index < 1) {
    digitalWrite(D1, isConnected);
  } else {
    digitalWrite(D2, isConnected);
  }
}
