#include "UbidotsEsp32Mqtt.h"
#include "WiFi.h"

#include <Preferences.h>

Preferences preferences;
#define DEVICE_LABEL "agroindustria"
//#define WIFISSID "INFINITUM4304_2.4" // WifiSSID
//#define PASSWORD "vxMr7ddEx3"// Password
//const char *WIFISSID = "Brando's S22 Ultra"; // WifiSSID-Brando's S22 Ultra
//const char *PASSWORD = "007brando";// Password
String WIFISSID = "";
String PASSWORD = "";

const char *ssidL = "AgroDevice1084";
const char *passwordL = "agro-007";

int minConduct = 0;
int maxConduct = 0;
int minOxy = 0;
int maxOxy = 0;
int minTemp = 0;
int maxTemp = 0;
int minPH = 0;
int maxPH = 0;

#define TOKEN "BBFF-R2QwDHPW5ap3FLDh2Q8A9Pv9iJXeNn"  //Token relacionado con el dispositivo Ubidots


float oxygenValue;
Ubidots ubidots(TOKEN);

void ubi_mainSetup() {

  preferences.begin("network", false);
  WIFISSID = preferences.getString("ssid");
  PASSWORD = preferences.getString("password");
  preferences.end();

  preferences.begin("conductivity", false);
  minConduct = preferences.getUInt("min");
  maxConduct = preferences.getUInt("max");
  preferences.end();

  preferences.begin("oxygen", false);
  minOxy = preferences.getUInt("min");
  maxOxy = preferences.getUInt("max");
  preferences.end();

  preferences.begin("ph", false);
  minPH = preferences.getUInt("min");
  maxPH = preferences.getUInt("max");
  preferences.end();

  preferences.begin("temperature", false);
  minTemp = preferences.getUInt("min");
  maxTemp = preferences.getUInt("max");
  preferences.end();

  printAllPreferences();

  pinMode(btnServer, INPUT_PULLUP);
  pinMode(btnBlower, INPUT_PULLUP);

  ubidots.setCallback(callback);
  ubidots.setup();
  Connect();
  //ubidots.connectToWifi(WIFISSID, PASSWORD);
  //delay(1000);
  ubidots.reconnect();


  timer_conductivity = millis();
  timer_temperature = millis();
  timer_oxygen = millis();
  timer_ph = millis();

  for (uint8_t i = 0; i < NUMBER_OF_VARIABLES; i++)
  {
    ubidots.subscribeLastValue(DEVICE_LABEL, variable_labels[i]); // Insert the device and Variable's Labels
    delay(100);
  }
  showLocalValues();
  ubi_verifyExistFiles();
  //GetAllValues();
}

void ubi_mainLoop() {
  showLocalValues();
  getAllElements();
  ubidots.loop();
}
void getAllElements() {
  if (!ubidots.connected())
  {
    ubidots.disconnect();
    Connect();
    //ubidots.reconnect();
    for (uint8_t i = 0; i < NUMBER_OF_VARIABLES; i++)
    {
      ubidots.subscribeLastValue(DEVICE_LABEL, variable_labels[i]); // Insert the device and Variable's Labels
      delay(100);
    }
    Serial.println("reconectado");
    ubi_verifyExistFiles();
  }
  GetAllValuesIntervals();
}
void GetAllValuesIntervals() {
  if (abs(millis() - timer_oxygen) > interval_oxygen)
  {
    GetOxy();
  }
  if (abs(millis() - timer_conductivity) > interval_conductivity)
  {
    GetCon();
  }
  if (abs(millis() - timer_temperature) > interval_temperature)
  {
    GetTemp();
  }
  if (abs(millis() - timer_ph) > interval_ph)
  {
    GetPH();
  }
}
void GetTemp() {
  int temperatura = random(0, 50);
  if (!ubidots.connected()) {
    DateTime now = rtc.now();
    String date = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " - " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
    String mensaje = String(now.unixtime() + 18000) + "," + date + "," + String(temperatura) + "\r\n";
    File tempFile = SD.open("/temperature.csv");
    if (tempFile) {
      appendFile(SD, "/temperature.csv", mensaje.c_str());
    } else {
      writeFile(SD, "/temperature.csv",  mensaje.c_str());
    }
  } else {
    ubidots.add("temperature", temperatura);
    ubidots.publish(DEVICE_LABEL);
  }
  timer_temperature = millis();
}

void GetOxy() {
  oxygenValue = GetOxygen();
  if (!ubidots.connected()) {
    DateTime now = rtc.now();
    String date = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " - " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
    String mensaje = String(now.unixtime() + 18000) + "," + date + "," + String(oxygenValue) + "\r\n";
    File oxiFile = SD.open("/oxygen.csv");
    if (oxiFile) {
      appendFile(SD, "/oxygen.csv", mensaje.c_str());
    } else {
      writeFile(SD, "/oxygen.csv",  mensaje.c_str());
    }
  } else {
    ubidots.add("oxygen", oxygenValue);
    ubidots.publish(DEVICE_LABEL);
  }
  timer_oxygen = millis();
}

void GetCon() {
  int randomCon = random(0, 50);
  if (!ubidots.connected()) {
    DateTime now = rtc.now();
    String date = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " - " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
    String mensaje = String(now.unixtime() + 18000) + "," + date + "," + String(randomCon) + "\r\n";
    File conFile = SD.open("/conductivity.csv");
    if (conFile) {
      appendFile(SD, "/conductivity.csv", mensaje.c_str());
    } else {
      writeFile(SD, "/conductivity.csv",  mensaje.c_str());
    }
  } else {
    ubidots.add("conductivity", randomCon);
    ubidots.publish(DEVICE_LABEL);
  }
  timer_conductivity = millis();
}

void GetPH() {
  int ph = random(0, 50);
  if (!ubidots.connected()) {
    DateTime now = rtc.now();
    String date = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + " - " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
    String mensaje = String(now.unixtime() + 18000) + "," + date + "," + String(ph) + "\r\n";
    File phFile = SD.open("/ph.csv");
    if (phFile) {
      appendFile(SD, "/ph.csv", mensaje.c_str());
    } else {
      writeFile(SD, "/ph.csv",  mensaje.c_str());
    }
  } else {
    ubidots.add("ph", ph);
    ubidots.publish(DEVICE_LABEL);
  }
  timer_ph = millis();
}
void ubi_verifyExistFiles() {
  File oxyFile = SD.open("/oxygen.csv");
  File tempFile = SD.open("/temperature.csv");
  File conFile = SD.open("/conductivity.csv");
  File phFile = SD.open("/ph.csv");

  if (oxyFile) {
    //lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Sincronizando  ");
    lcd.setCursor(0, 1);
    lcd.print("Datos de oxigeno");
    readFileLineByLine(SD, "/oxygen.csv", Oxygen);
    lcd.setCursor(0, 0);
    lcd.print("      datos     ");
    lcd.setCursor(0, 1);
    lcd.print(" sincronizandos ");
    delay(100);
    lcd.clear();
  }

  if (tempFile) {
    lcd.setCursor(0, 0);
    lcd.print(" Sincronizando  ");
    lcd.setCursor(0, 1);
    lcd.print("Datos de temper_");
    readFileLineByLine(SD, "/temperature.csv", Temperature);
    lcd.setCursor(0, 0);
    lcd.print("      datos     ");
    lcd.setCursor(0, 1);
    lcd.print(" sincronizandos ");
    delay(100);
    lcd.clear();
  }

  if (conFile) {
    lcd.setCursor(0, 0);
    lcd.print(" Sincronizando  ");
    lcd.setCursor(0, 1);
    lcd.print("datos de conduc_");
    readFileLineByLine(SD, "/conductivity.csv", Conductivity);
    lcd.setCursor(0, 0);
    lcd.print("      datos     ");
    lcd.setCursor(0, 1);
    lcd.print(" sincronizandos ");
    delay(100);
    lcd.clear();
  }

  if (phFile) {
    lcd.setCursor(0, 0);
    lcd.print(" Sincronizando  ");
    lcd.setCursor(0, 1);
    lcd.print("datos de ph     ");
    readFileLineByLine(SD, "/ph.csv", Ph);
    lcd.setCursor(0, 0);
    lcd.print("      datos     ");
    lcd.setCursor(0, 1);
    lcd.print(" sincronizandos ");
    delay(100);
    lcd.clear();
  }

  GetAllValues();
}

void readFileLineByLine(fs::FS &fs, const char * path, const char * var) {
  //Serial.printf("Reading file: %s\n", path);
  int c = 0;
  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  //char* ubiContext = "\"estado\":\"resuvido\"";
  char* ubiContext = "";
  Serial.print("Read from file: ");
  while (file.available()) {
    //if (abs(millis() - timer_upload) > interval_upload) {
    String buffer = "";
    buffer = file.readStringUntil('\n');
    int i1 = buffer.indexOf(',');
    int i2 = buffer.indexOf(',', i1 + 1);

    String unix = buffer.substring(0, i1);
    unsigned long ubiDate = atol(unix.c_str());
    String d = buffer.substring(i1 + 1, i2);
    String v = buffer.substring(i2 + 1);
    float ubiValue = v.toFloat();
    Serial.print("U: ");
    Serial.print(ubiDate);
    Serial.print(" D: ");
    Serial.print(d);
    Serial.print(" V: ");
    Serial.println(ubiValue);
    delay(500);
    ubidots.add(var, ubiValue, ubiContext, ubiDate);
    ubidots.publish(DEVICE_LABEL);
    ubidots.loop();
    c++;
    //timer_upload = millis();
    //}
  }
  Serial.print("Número de elementos sincronizados: ");
  Serial.println(c);
  file.close();
  deleteFile(SD, path);
}

void Connect() {
  GetAllValues();
  const long interval = 2000;
  WiFi.begin(WIFISSID.c_str(), PASSWORD.c_str());

  unsigned long currentMillis = millis();
  while (WiFi.status() != WL_CONNECTED) {
    showLocalValues();
    GetAllValuesIntervals();
    if (abs(millis() - currentMillis) > interval) {
      /*lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("  Conectando a  ");
        lcd.setCursor(0, 1);
        lcd.print(WIFISSID);*/
      //conectado = false;
      Serial.print(".");
      currentMillis = millis();
      //break;
    }
    //ubi_NoConnection();
  }
  while (WiFi.status() == WL_CONNECTED) {
    showLocalValues();
    GetAllValuesIntervals();
    if (abs(millis() - currentMillis) > interval) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Conexion a red  ");
      if (ubidots.reconnect()) {
        break;
      }
      currentMillis = millis();
      lcd.setCursor(0, 1);
      lcd.print(" sin internet    ");
    }
  }
  delay(500);
  lcd.clear();
  //showLocalValues();
  GetAllValues();
}

void showLocalValues() {
  btnBlowerState = digitalRead(btnBlower);
  btnServerState = digitalRead(btnServer);

  if (!btnServerState) {
    timer_to_start_server = millis();
    delay(1000);
    btnServerState = digitalRead(btnServer);
    if (!btnServerState) {
      serverSetup();
      //////////
      WiFi.disconnect();
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssidL, passwordL);

      IPAddress IP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(IP);
      //server.begin();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Dispositivo en ");
      lcd.setCursor(0, 1);
      lcd.print("  config_mode   ");
      //////////
      while (true) {

        btnServerState = digitalRead(btnServer);
        if (btnServerState) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("   Reiniciando  ");
          lcd.setCursor(0, 1);
          lcd.print("   dispositivo  ");
          delay(1000);
          break;
        }
      }
      ESP.restart();
    }
  }

  if (!btnBlowerState) {
    if (abs(millis() - timer_pullup) > interval_pullup)
    {
      if (BlowerState) {
        if (ubidots.connected()) {
          ubidots.add("control1", valorEncendido);
          ubidots.publish(DEVICE_LABEL);
        }
        digitalWrite(R2, HIGH);
        digitalWrite(R1, HIGH);
      } else {
        if (ubidots.connected()) {
          ubidots.add("control1", valorApagado);
          ubidots.publish(DEVICE_LABEL);
        }
        digitalWrite(R1, LOW);
        digitalWrite(R2, LOW);
      }
      BlowerState = !BlowerState;
      Serial.print("Blower ");
      Serial.println(BlowerState);
      timer_pullup = millis();
    }
  }
  if (abs(millis() - timer_local_check) > interval_local_check)
  {
    oxygenValue = GetOxygen();

    if (oxygenValue <= minOxy) {
      digitalWrite(R1, HIGH);
    } else if (oxygenValue >= maxOxy) {
      digitalWrite(R1, LOW);
    }
    lcd.setCursor(0, 0);
    lcd.print("Ox:  ");
    lcd.setCursor(4, 0);
    lcd.print(oxygenValue);

    lcd.setCursor(0, 1);
    lcd.print("Co: ");
    lcd.setCursor(4, 1);
    lcd.print(tdsValue);

    if (tdsValue >= 50) {

    }

    lcd.setCursor(9, 0);
    lcd.print("Te:");
    lcd.setCursor(13, 0);
    lcd.print(temp);

    if (temp >= 50) {

    }

    lcd.setCursor(9, 1);
    lcd.print("PH:");
    lcd.setCursor(13, 1);
    lcd.print(random(1, 5));

    timer_local_check = millis();
  }

  /*
    if (abs(millis() - timer_sensors_loop) > interval_sensors_loop)
    {

      timer_sensors_loop = millis();
    }*/
}

void GetAllValues() {
  GetOxy();
  GetTemp();
  GetCon();
  GetPH();
}


void printAllPreferences() {
  Serial.println("SSID: " + WIFISSID);
  Serial.println("PASS: " + PASSWORD);
  Serial.println("Conductivity min: " + String(minConduct));
  Serial.println("Conductivity max: " + String(maxConduct));
  Serial.println("Oxygen min: " + String(minOxy));
  Serial.println("Oxygen max: " + String(maxOxy));
  Serial.println("PH min: " + String(minPH));
  Serial.println("PH max: " + String(maxPH));
  Serial.println("Temperature min: " + String(minTemp));
  Serial.println("Temperature max: " + String(maxTemp));
}
