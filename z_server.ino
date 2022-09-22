#include "ESPAsyncWebServer.h"
#include <ArduinoJson.h>

AsyncWebServer server(80);

void serverSetup() {
  server.on("/checkDevice", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);

    json["name"] = deviceName;
    json["type"] = deviceType;

    serializeJson(json, *response);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/getLastValues", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);

    json["oxygen"] = GetOxygen();
    json["temperature"] = temperaturaLoop();
    json["conductivity"] = conductivityLoop();
    json["ph"] = phloop();

    json["minOxy"] = minOxy;
    json["maxOxy"] = maxOxy;
    json["minTemp"] = minTemp;
    json["maxTemp"] = maxTemp;
    json["minPH"] = minPH;
    json["maxPH"] = maxPH;
    json["minConduct"] = minConduct;
    json["maxConduct"] = maxConduct;

    serializeJson(json, *response);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/getAllElementValues", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1176);

    preferences.begin("valuesOx", false);
    json["oxygen"].add(preferences.getFloat("o5"));
    json["oxygen"].add(preferences.getFloat("o4"));
    json["oxygen"].add(preferences.getFloat("o3"));
    json["oxygen"].add(preferences.getFloat("o2"));
    json["oxygen"].add(preferences.getFloat("o1"));
    preferences.end();

    preferences.begin("valuesPh", false);
    json["ph"].add(preferences.getFloat("ph5"));
    json["ph"].add(preferences.getFloat("ph4"));
    json["ph"].add(preferences.getFloat("ph3"));
    json["ph"].add(preferences.getFloat("ph2"));
    json["ph"].add(preferences.getFloat("ph1"));
    preferences.end();

    preferences.begin("valuesTe", false);
    json["temp"].add(preferences.getFloat("t5"));
    json["temp"].add(preferences.getFloat("t4"));
    json["temp"].add(preferences.getFloat("t3"));
    json["temp"].add(preferences.getFloat("t2"));
    json["temp"].add(preferences.getFloat("t1"));
    preferences.end();

    preferences.begin("valuesCon", false);
    json["cond"].add(preferences.getFloat("c5"));
    json["cond"].add(preferences.getFloat("c4"));
    json["cond"].add(preferences.getFloat("c3"));
    json["cond"].add(preferences.getFloat("c2"));
    json["cond"].add(preferences.getFloat("c1"));
    preferences.end();

    preferences.begin("valuesD", false);
    json["dates"].add(preferences.getString("d1_5"));
    json["dates"].add(preferences.getString("d1_4"));
    json["dates"].add(preferences.getString("d1_3"));
    json["dates"].add(preferences.getString("d1_2"));
    json["dates"].add(preferences.getString("d1_1"));
    preferences.end();
    json["status"] = "ok";

    json["minOxy"] = minOxy;
    json["maxOxy"] = maxOxy;
    json["minTemp"] = minTemp;
    json["maxTemp"] = maxTemp;
    json["minPH"] = minPH;
    json["maxPH"] = maxPH;
    json["minConduct"] = minConduct;
    json["maxConduct"] = maxConduct;

    serializeJson(json, *response);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/getFile", HTTP_GET, [](AsyncWebServerRequest *request) {
    int paramsNr = request->params();
    String fileName = "";
    for (int i = 0; i < paramsNr; i++) {
      AsyncWebParameter *p = request->getParam(i);
      if (p->name() == "fileName") {
        fileName = p->value();
      }
    }
    File file = SD.open("/" + fileName);

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

  server.on("/getAllFileNames", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
      json["msjsd"] = "Tarjeta SD no encontrada. -- ";
    } else {
      json["msjsd"] = "";
    }
    if (ubidots.connected()) {
      json["msjnet"] = "Dispositivo conectado a internet.";
    } else {
      json["msjnet"] = "Dispositivo sin conexión a internet.";
    }

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

  server.on("/emptyFile", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);

    int paramsNr = request->params();
    String fileName = "";
    for (int i = 0; i < paramsNr; i++) {
      AsyncWebParameter *p = request->getParam(i);
      if (p->name() == "fileName") {
        fileName = "/" + p->value();
      }
    }
    File file = SD.open("/" + fileName);

    if (file) {
      writeFile(SD, fileName.c_str(), "");
      //file.write("");
      json["status"] = "Archivo vacío.";
    } else {
      json["status"] = "No existe el archivo.";
      Serial.println("No existe el archivo oxygen");
    }
    //archivos += "]";
    //json["files"] = archivos;
    json["status"] = "ok";
    serializeJson(json, *response);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/changeParams", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");

    String ssidN = "";
    String passN = "";
    int minConductn = minConduct;
    int maxConductn = maxConduct;
    int minOxyn = minOxy;
    int maxOxyn = maxOxy;
    int minTempn = minTemp;
    int maxTempn = maxTemp;
    int minPHn = minPH;
    int maxPHn = maxPH;

    DynamicJsonDocument json(1024);
    int paramsNr = request->params();
    for (int i = 0; i < paramsNr; i++) {
      AsyncWebParameter *p = request->getParam(i);
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

  server.on("/getParams", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);

    json["ssid"] = WIFISSID;
    json["pass"] = PASSWORD;
    json["minConduct"] = String(minConduct);
    json["maxConduct"] = String(maxConduct);
    json["minOxy"] = String(minOxy);
    json["maxOxy"] = String(maxOxy);
    json["minTemp"] = String(minTemp);
    json["maxTemp"] = String(maxTemp);
    json["minPH"] = String(minPH);
    json["maxPH"] = String(maxPH);
    serializeJson(json, *response);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
    Serial.println("Params returned");
  });

  server.on("/getOutOfRangeValues", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(1024);
    int contador = 0;

    if (lvOx < minOxy || lvOx > maxOxy) {
      Serial.println(lvOx);
      Serial.println(minOxy);
      json["oxygen"] = lvOx;
      contador++;
    }
    if (lvTe < minTemp || lvTe > maxTemp) {
      Serial.println(lvTe);
      Serial.println(minTemp);
      json["temperature"] = lvTe;
      contador++;
    }
    if (lvCo < minConduct || lvCo > maxConduct) {
      Serial.println(lvCo);
      Serial.println(minConduct);
      json["conductividad"] = lvCo;
      contador++;
    }
    if (lvPh < minPH || lvPh > maxPH) {
      Serial.println(lvPh);
      Serial.println(minPH);
      json["ph"] = lvPh;
      contador++;
    }

    json["count"] = contador;

    serializeJson(json, *response);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
    Serial.println("Params returned");
  });

  bool status;

  // Start server
  server.begin();
}