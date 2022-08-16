#include "ESPAsyncWebServer.h"
#include <ArduinoJson.h>

AsyncWebServer server(80);

void serverSetup() {
  server.on("/checkDevice", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);

    json["name"] = deviceName;
    json["type"] = deviceType;

    serializeJson(json, *response);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });
  
  server.on("/getAllElementValues", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);

    json["oxygen"].add(random(0,50));
    json["oxygen"].add(random(0,50));
    json["oxygen"].add(random(0,50));
    json["oxygen"].add(random(0,50));
    json["oxygen"].add(random(0,50));
    json["dateOxygen"].add("05:10");
    json["dateOxygen"].add("05:15");
    json["dateOxygen"].add("05:20");
    json["dateOxygen"].add("05:25");
    json["dateOxygen"].add("05:30");
    json["temp"].add(random(50,100));
    json["temp"].add(random(50,100));
    json["temp"].add(random(50,50));
    json["temp"].add(random(50,50));
    json["temp"].add(random(50,50));
    json["dateTemp"].add("05:10");
    json["dateTemp"].add("05:15");
    json["dateTemp"].add("05:20");
    json["dateTemp"].add("05:25");
    json["dateTemp"].add("05:30");
    json["ph"].add(random(150,200));
    json["ph"].add(random(150,200));
    json["ph"].add(random(150,200));
    json["ph"].add(random(150,200));
    json["ph"].add(random(150,200));
    json["datePh"].add("05:10");
    json["datePh"].add("05:15");
    json["datePh"].add("05:20");
    json["datePh"].add("05:25");
    json["datePh"].add("05:30");
    json["cond"].add(random(250,500));
    json["cond"].add(random(250,500));
    json["cond"].add(random(250,500));
    json["cond"].add(random(250,500));
    json["cond"].add(random(250,500));
    json["dateCond"].add("05:10");
    json["dateCond"].add("05:15");
    json["dateCond"].add("05:20");
    json["dateCond"].add("05:25");
    json["dateCond"].add("05:30");
    json["status"] = "ok";

    serializeJson(json, *response);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/getFileOxygen", HTTP_GET, [](AsyncWebServerRequest * request) {

    File testFile = SD.open("/oxygen1.csv");
    AsyncWebServerResponse *response = request->beginResponse(testFile, "/oxygen1.csv", "text/xhr", true);

    DynamicJsonDocument json(1024);
    int paramsNr = request->params();
    if (testFile) {
      Serial.println("Si existe el archivo");
    } else {
      Serial.println("No existe el archivo");
    }
    json["name"] = deviceName;
    json["type"] = deviceType;

    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/getFile", HTTP_GET, [](AsyncWebServerRequest * request) {
    int paramsNr = request->params();
    String fileName = "";
    for (int i = 0; i < paramsNr; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->name() == "fileName") {
        fileName = p->value();
      }
    }
    File file = SD.open("/"+fileName);

    if (file) {
      Serial.println("Si existe el archivo");
      AsyncWebServerResponse *response = request->beginResponse(file, fileName, "text/xhr", true);
      request->send(response);
    } else {
      AsyncResponseStream *response = request->beginResponseStream("application/json");
      response->addHeader("Access-Control-Allow-Origin", "*");
      DynamicJsonDocument json(1024);
      json["error"] = "archivo no encontrado";
      serializeJson(json, *response);
      response->addHeader("Access-Control-Allow-Origin", "*");
      Serial.println("No existe el archivo");
      request->send(response);
    }
  });

  server.on("/getAllFileNames", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);

    File oxygenFile = SD.open("/oxygen.csv");
    File phFile = SD.open("/ph.csv");
    File temperatureFile = SD.open("/temperature.csv");
    File conductivityFile = SD.open("/conductivity.csv");
    File testFile = SD.open("/test.csv");
    //String archivos = "[";
    if (oxygenFile) {
      json["files"].add("oxygen.csv");
    } else {
      Serial.println("No existe el archivo oxygen");
    }
    if (phFile) {
      json["files"].add("ph.csv");
    } else {
      Serial.println("No existe el archivo ph");
    }
    if (temperatureFile) {
      json["files"].add("temperature.csv");
    } else {
      Serial.println("No existe el archivo temperature");
    }
    if (conductivityFile) {
      json["files"].add("conductivity.csv");
    } else {
      Serial.println("No existe el archivo conductivity");
    }
    if (testFile) {
      json["files"].add("test.csv");
    } else {
      Serial.println("No existe el archivo test");
    }
    //archivos += "]";
    //json["files"] = archivos;
    json["status"] = "ok";
    serializeJson(json, *response);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });
  
  server.on("/emptyFile", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);

    int paramsNr = request->params();
    String fileName = "";
    for (int i = 0; i < paramsNr; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->name() == "fileName") {
        fileName = "/" + p->value();
      }
    }
    File file = SD.open("/"+fileName);

    if (file) {
      writeFile(SD, fileName.c_str(), "");
      //file.write("");
      json["status"] =  "Archivo vacío.";
    } else {
      json["status"] =  "No existe el archivo.";
      Serial.println("No existe el archivo oxygen");
    }
    //archivos += "]";
    //json["files"] = archivos;
    json["status"] = "ok";
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
