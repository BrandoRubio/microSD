#include "UbidotsEsp32Mqtt.h"
#include "WiFi.h"

#define DEVICE_LABEL "dispositivo_agroendustrial"
String WIFISSID = "";
String PASSWORD = "";
String deviceName = "";
String deviceType = "";

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
float temp = 0;
boolean flag21 = false;
boolean flag22 = false;
boolean flag23 = false;
boolean flag24 = false;

#define TOKEN "BBFF-sKey93H2cjEIllE2byhoEXJJYiedHk"  //Token relacionado con el dispositivo Ubidots

float oxygenValue;
float Conductividad;
float Temperatura, ph;
Ubidots ubidots(TOKEN);

void ubi_mainSetup() {

  preferences.begin("network", false);
  WIFISSID = preferences.getString("ssid");
  PASSWORD = preferences.getString("password");
  preferences.end();

  preferences.begin("data", false);
  deviceName = preferences.getString("name");
  deviceType = preferences.getString("type");
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

  // printAllPreferences();
  pinMode(bocina, OUTPUT);
  pinMode(btnServer, INPUT_PULLUP);
  pinMode(btnBlower, INPUT_PULLUP);

  ubidots.setCallback(callback);
  ubidots.setup();
  Connect();

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
}

void ubi_mainLoop() {
  conexionwifi();
  showLocalValues();
  getAllElements();
  ubidots.loop();
}
void getAllElements() {
  if (!ubidots.connected())
  {
    ubidots.disconnect();

    Connect();
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
  if (abs(millis() - timer_ten_minutes) > interval_ten_minutes)
  {
    OxygenNewValue(GetOxygen());
    TempNewValue(temperaturaLoop());
    ConNewValue(conductivityLoop());
    PhNewValue(phloop());
    DateTime nowd = rtc.now();
    newDateValue(String(nowd.timestamp(DateTime::TIMESTAMP_FULL)));
    timer_ten_minutes = millis();
  }
}
void GetTemp() {
  Temperatura = temperaturaLoop();
  //int temperatura = random(0, 50);
  if (!ubidots.connected()) {
    DateTime now = rtc.now();
    String date = now.timestamp(DateTime::TIMESTAMP_FULL);
    String mensaje = String(now.unixtime()) + "," + date + "," + String(Temperatura) + "\r\n";
    File tempFile = SD.open("/temperature.csv");
    if (tempFile) {
      appendFile(SD, "/temperature.csv", mensaje.c_str());
    } else {
      writeFile(SD, "/temperature.csv",  mensaje.c_str());
    }
  } else {
    ubidots.add("temperature", Temperatura);
    ubidots.publish(DEVICE_LABEL);
  }
  timer_temperature = millis();
}

void GetOxy() {
  oxygenValue = GetOxygen();
  if (!ubidots.connected()) {
    DateTime now = rtc.now();
    String date =  now.timestamp(DateTime::TIMESTAMP_FULL);
    String mensaje = String(now.unixtime()) + "," + date + "," + String(oxygenValue) + "\r\n";
    String mensaje2 = String(now.unixtime()) + "," + date + "," + String(random(0, 100)) + "\r\n";
    File oxiFile = SD.open("/oxygen.csv");
    if (oxiFile) {
      appendFile(SD, "/test.csv", mensaje2.c_str());
      appendFile(SD, "/oxygen.csv", mensaje.c_str());
    } else {
      appendFile(SD, "/test.csv", mensaje2.c_str());
      writeFile(SD, "/oxygen.csv",  mensaje.c_str());
    }
  } else {
    ubidots.add("oxygen", oxygenValue);
    ubidots.publish(DEVICE_LABEL);
  }
  timer_oxygen = millis();
}

void GetCon() {
  Conductividad = conductivityLoop();
  //int randomCon = random(0, 50);
  if (!ubidots.connected()) {
    DateTime now = rtc.now();
    String date =  now.timestamp(DateTime::TIMESTAMP_FULL);
    String mensaje = String(now.unixtime()) + "," + date + "," + String(Conductividad) + "\r\n";
    File conFile = SD.open("/conductivity.csv");
    if (conFile) {
      appendFile(SD, "/conductivity.csv", mensaje.c_str());
    } else {
      writeFile(SD, "/conductivity.csv",  mensaje.c_str());
    }
  } else {
    ubidots.add("conductivity", Conductividad);
    ubidots.publish(DEVICE_LABEL);
  }
  timer_conductivity = millis();
}

void GetPH() {
  ph = phloop();
  //  int ph = random(0, 50);
  if (!ubidots.connected()) {
    DateTime now = rtc.now();
    String date =  now.timestamp(DateTime::TIMESTAMP_FULL);
    String mensaje = String(now.unixtime()) + "," + date + "," + String(ph) + "\r\n";
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
  lcd.clear();

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
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Conectando a  ");
  lcd.setCursor(0, 1);
  lcd.print(WIFISSID);

  unsigned long currentMillis = millis();
  while (WiFi.status() != WL_CONNECTED) {
    showLocalValues();
    GetAllValuesIntervals();
    desconexionwifi();
    if (abs(millis() - currentMillis) > interval) {
      Serial.print(".");
      currentMillis = millis();

    }
  }
  Serial.println(WiFi.localIP());
  serverSetup();
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

  if (flag21 == false) {
  }
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
          ubidots.add("control1", 0);
          ubidots.publish(DEVICE_LABEL);
        }
        BlowerState = false;
        digitalWrite(R1, LOW);
        digitalWrite(R2, LOW);
      } else {
        if (ubidots.connected()) {
          ubidots.add("control1", 1);
          ubidots.publish(DEVICE_LABEL);
        }
        BlowerState = true;
        digitalWrite(R2, HIGH);
        digitalWrite(R1, HIGH);
      }
      //BlowerState = !BlowerState;
      Serial.print("Blower ");
      Serial.println(BlowerState);
      timer_pullup = millis();
    }
  }

  if (abs(millis() - timer_local_check) > interval_local_check)
  {
    //lcd.clear();
    oxygenValue = GetOxygen();
    if (oxygenValue == 0) {

    } else if (oxygenValue <= minOxy) {
      lcd.createChar(1, customChar1);
      lcd.setCursor(2, 0);
      lcd.write(1);
      digitalWrite(R1, LOW);
      digitalWrite(R2, LOW);
      BlowerState = false;
      Serial.print("Blower ");
      Serial.println(BlowerState);
      //timer_pullup = millis();
      if (ubidots.connected()) {
        ubidots.add("control1", 1);
        ubidots.publish(DEVICE_LABEL);
      }
    } else if (oxygenValue >= maxOxy) {

      lcd.createChar(0, customChar);
      lcd.setCursor(2, 0);
      lcd.write(0);
      digitalWrite(R1, HIGH);
      digitalWrite(R2, HIGH);
      BlowerState = true;
      Serial.print("Blower ");
      Serial.println(BlowerState);
      //timer_pullup = millis();
      if (ubidots.connected()) {
        ubidots.add("control1", 0);
        ubidots.publish(DEVICE_LABEL);
      }
    }
    lcd.setCursor(0, 0);
    lcd.print("Ox");
    lcd.setCursor(3, 0);
    lcd.print(String(oxygenValue) + "   ");


    lcd.setCursor(0, 1);
    lcd.print("Co");
    lcd.setCursor(3, 1);
    lcd.print(Conductividad);

    if (tdsValue <= minConduct ) {
      lcd.createChar(4, customChar1);
      lcd.setCursor(2, 1);
      lcd.write(4);
    }
    if (tdsValue >= maxConduct ) {
      lcd.createChar(3, customChar);
      lcd.setCursor(2, 1);
      lcd.write(3);

    }
    if (tdsValue >= minConduct && tdsValue <= maxConduct ) {
      lcd.setCursor(2, 1);
      lcd.print(" ");
    }
    lcd.setCursor(12, 0);
    lcd.print("Te");
    lcd.setCursor(15, 0);
    lcd.print(Temperatura, 2);

    if (Temperatura >= maxTemp && flag21 == false ) {
      activarbocina();
      lcd.setCursor(14, 0);
      lcd.print(" ");
      lcd.createChar(0, customChar);
      lcd.setCursor(14, 0);
      lcd.write(0);
    }
    if (Temperatura <= minTemp && flag21 == false ) {
      activarbocina();
      lcd.setCursor(14, 0);
      lcd.print(" ");
      lcd.createChar(1, customChar1);
      lcd.setCursor(14, 0);
      lcd.write(1);
    }
    if (Temperatura >= minTemp && Temperatura <= maxTemp && flag21 == true ) {
      desactivarbocina();
      lcd.setCursor(12, 0);
      lcd.print(" ");
    }

    lcd.setCursor(12, 1);
    lcd.print("PH");
    lcd.setCursor(15, 1);
    lcd.print(ph);

    timer_local_check = millis();
  }

}

void GetAllValues() {
  GetOxy();
  GetTemp();
  GetCon();
  GetPH();
}
void activarbocina() {
  digitalWrite(bocina, HIGH);
  flag21 = true;
}

void desactivarbocina() {
  digitalWrite(bocina, LOW);
  flag21 = false;
}
void conexionwifi() {
  lcd.createChar(2, wifi);
  lcd.setCursor(19, 3);
  lcd.write(2);
  delay(500);
  lcd.createChar(2, wifi1);
  lcd.setCursor(19, 3);
  lcd.write(2);
  delay(500);
  lcd.createChar(2, wifi2);
  lcd.setCursor(19, 3);
  lcd.write(2);
  delay(500);
  lcd.createChar(2, wifi3);
  lcd.setCursor(19, 3);
  lcd.write(2);
  delay(500);
  lcd.createChar(2, wifi4);
  lcd.setCursor(19, 3);
  lcd.write(2);
  delay(500);
  lcd.setCursor(19, 3);
  lcd.print(" ");
  delay(500);
}
void desconexionwifi() {
  lcd.createChar(2, wifi5);
  lcd.setCursor(19, 3);
  lcd.write(2);
  delay(500);
  lcd.setCursor(19, 3);
  lcd.print(" ");
  delay(500);
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
