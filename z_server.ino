#include "ESPAsyncWebServer.h"
#include <ArduinoJson.h>

AsyncWebServer server(80);

void serverSetup() {
  server.on("/checkDevice", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);
    int paramsNr = request->params();

    json["name"] = String(ssidL);
    serializeJson(json, *response);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/changeParams", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");

    String ssidN = "";
    String passN = "";
    int minConductn = 0;
    int maxConductn = 0;
    int minOxyn = 0;
    int maxOxyn = 0;
    int minTempn = 0;
    int maxTempn = 0;
    int minPHn = 0;
    int maxPHn = 0;

    DynamicJsonDocument json(1024);
    int paramsNr = request->params();
    for (int i = 0; i < paramsNr; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->name() == "ssid") {
        ssidN = p->value();
      }
      if (p->name() == "password") {
        passN = p->value();
      }
      if (p->name() == "minc") {
        minConductn = p->value().toInt();
      }
      if (p->name() == "maxc") {
        maxConductn = p->value().toInt();
      }
      if (p->name() == "mino") {
        minOxyn = p->value().toInt();
      }
      if (p->name() == "maxo") {
        maxOxyn = p->value().toInt();
      }
      if (p->name() == "mint") {
        minTempn = p->value().toInt();
      }
      if (p->name() == "maxt") {
        maxTempn = p->value().toInt();
      }
      if (p->name() == "minp") {
        minPHn = p->value().toInt();
      }
      if (p->name() == "maxp") {
        maxPHn = p->value().toInt();
      }
    }

    json["status"] = "ok";
    serializeJson(json, *response);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);


    preferences.begin("network", false);
    preferences.putString("ssid", ssidN);
    preferences.putString("password", passN);
    preferences.end();
    preferences.begin("conductivity", false);
    preferences.putUInt("min", minConductn);
    preferences.putUInt("max", maxConductn);
    preferences.end();
    preferences.begin("oxygen", false);
    preferences.putUInt("min", minOxyn);
    preferences.putUInt("max", maxOxyn);
    preferences.end();
    preferences.begin("temperature", false);
    preferences.putUInt("min", minTempn);
    preferences.putUInt("max", maxTempn);
    preferences.end();
    preferences.begin("ph", false);
    preferences.putUInt("min", minPHn);
    preferences.putUInt("max", maxPHn);
    preferences.end();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   Parametros   ");
    lcd.setCursor(0, 1);
    lcd.print("   Recibidos    ");
    delay(500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   Reiniciando  ");
    lcd.setCursor(0, 1);
    lcd.print("   dispositivo  ");
    delay(1000);
    ESP.restart();

  });

  server.on("/getParams", HTTP_GET, [](AsyncWebServerRequest * request) {

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);

    json["ssid"] = WIFISSID;
    json["pass"] = PASSWORD;
    json["minConduct"] = String(minConduct);
    json["maxConduct"] = String(maxConduct);
    json["minOxy"] = String(minOxy);
    json["maxOxy"] = String(maxOxy);
    json["minTemp"] = String(minTemp);
    json["maxTemp"] = String(minTemp);
    json["minPH"] = String(minPH);
    json["maxPH"] = String(maxPH);
    serializeJson(json, *response);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
    Serial.println("Params returned");
  });

  bool status;

  // Start server
  server.begin();
}
