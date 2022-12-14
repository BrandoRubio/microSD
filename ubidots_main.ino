#include "UbidotsEsp32Mqtt.h"
#include "WiFi.h"

#define DEVICE_LABEL "prueba"
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
float lvOx, lvTe, lvCo, lvPh = 0;

boolean flag21 = false;

#define TOKEN "BBFF-vumPM2iRw56ECXCE6JhvxWiED5GLyv"  //Token relacionado con el dispositivo Ubidots

float oxygenValue;
float Conductividad;
float Temperatura, ph;
Ubidots ubidots(TOKEN);

void () {

  getAllPreferences();
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

  for (uint8_t i = 0; i < NUMBER_OF_VARIABLES; i++) {
    ubidots.subscribeLastValue(DEVICE_LABEL, variable_labels[i]);  // Insert the device and Variable's Labels
    delay(100);
  }
  showLocalValues();
  ubi_verifyExistFiles();
}

void ubi_mainLoop() {
  //conexionwifi();
  showLocalValues();
  getAllElements();
  ubidots.loop();
}
void getAllElements() {
  if (!ubidots.connected()) {
    dataLog("Desconectandose de ubidots", 0);
    ubidots.disconnect();

    Connect();
    for (uint8_t i = 0; i < NUMBER_OF_VARIABLES; i++) {
      ubidots.subscribeLastValue(DEVICE_LABEL, variable_labels[i]);  // Insert the device and Variable's Labels
      delay(100);
    }
    Serial.println("reconectado");
    dataLog("Reconectandose a ubidots", 0);
    ubi_verifyExistFiles();
  }
  conexionwifi();
  GetAllValuesIntervals();
}
void GetAllValuesIntervals() {
  dataLog("Revisando datos de los sensores", 0);
  if (abs(millis() - timer_oxygen) > interval_oxygen) {
    GetOxy();
  }
  if (abs(millis() - timer_conductivity) > interval_conductivity) {
    GetCon();
  }
  if (abs(millis() - timer_temperature) > interval_temperature) {
    GetTemp();
  }
  if (abs(millis() - timer_ph) > interval_ph) {
    GetPH();
  }
  if (abs(millis() - timer_save_data) > interval_save_data) {
    OxygenNewValue(lvOx);
    TempNewValue(lvTe);
    ConNewValue(lvCo);
    PhNewValue(lvPh);
    DateTime nowd = rtc.now();
    newDateValue(String(nowd.timestamp(DateTime::TIMESTAMP_FULL)));
    timer_save_data = millis();
  }
}
void GetTemp() {
  lvTe = temperaturaLoop();
  //int temperatura = random(0, 50);
  if (lvTe != -127) {
    if (!ubidots.connected()) {
      dataLog("Creando archivo temperatura", 0);
      DateTime now = rtc.now();
      String date = now.timestamp(DateTime::TIMESTAMP_FULL);
      String mensaje = String(now.unixtime()) + "," + date + "," + String(lvTe) + "\r\n";
      File tempFile = SD.open("/temperature.csv");
      if (tempFile) {
        appendFile(SD, "/temperature.csv", mensaje.c_str());
      } else {
        writeFile(SD, "/temperature.csv", mensaje.c_str());
      }
    } else {
      ubidots.add("temperature", lvTe);
      ubidots.publish(DEVICE_LABEL);
      dataLog("Ubidots publicacion de temperatura", 0);
    }
  }
  timer_temperature = millis();
}

void GetOxy() {
  lvOx = GetOxygen();
  if (lvOx != 0) {
    if (!ubidots.connected()) {
      dataLog("Creando archivo de Oxigenacion", 0);
      DateTime now = rtc.now();
      String date = now.timestamp(DateTime::TIMESTAMP_FULL);
      String mensaje = String(now.unixtime()) + "," + date + "," + String(lvOx) + "\r\n";
      String mensaje2 = String(now.unixtime()) + "," + date + "," + String(random(0, 100)) + "\r\n";
      File oxiFile = SD.open("/oxygen.csv");
      if (oxiFile) {
        appendFile(SD, "/test.csv", mensaje2.c_str());
        appendFile(SD, "/oxygen.csv", mensaje.c_str());
      } else {
        appendFile(SD, "/test.csv", mensaje2.c_str());
        writeFile(SD, "/oxygen.csv", mensaje.c_str());
      }
    } else {
      ubidots.add("oxygen", lvOx);
      ubidots.publish(DEVICE_LABEL);
      dataLog("Publicando Oxigenacion", 0);
    }
  }
  timer_oxygen = millis();
}

void GetCon() {
  lvCo = conductivityLoop();
  //int randomCon = random(0, 50);
  if (lvCo != 0) {
    if (!ubidots.connected()) {
      dataLog("Creando archivo Conductividad", 0);
      DateTime now = rtc.now();
      String date = now.timestamp(DateTime::TIMESTAMP_FULL);
      String mensaje = String(now.unixtime()) + "," + date + "," + String(lvCo) + "\r\n";
      File conFile = SD.open("/conductivity.csv");
      if (conFile) {
        appendFile(SD, "/conductivity.csv", mensaje.c_str());
      } else {
        writeFile(SD, "/conductivity.csv", mensaje.c_str());
      }
    } else {
      ubidots.add("conductivity", lvCo);
      ubidots.publish(DEVICE_LABEL);
      dataLog("Publicando conductividad", 0);
    }
  }
  timer_conductivity = millis();
}

void GetPH() {
  lvPh = phloop();
  //  int ph = random(0, 50);
  if (lvPh != 15.75) {
    if (!ubidots.connected()) {
      dataLog("Creando archivo de PH", 0);
      DateTime now = rtc.now();
      String date = now.timestamp(DateTime::TIMESTAMP_FULL);
      String mensaje = String(now.unixtime()) + "," + date + "," + String(lvPh) + "\r\n";
      File phFile = SD.open("/ph.csv");
      if (phFile) {
        appendFile(SD, "/ph.csv", mensaje.c_str());
      } else {
        writeFile(SD, "/ph.csv", mensaje.c_str());
      }
    } else {
      ubidots.add("ph", lvPh);
      ubidots.publish(DEVICE_LABEL);
      dataLog("Publicando PH", 0);
    }
  }
  timer_ph = millis();
}
void ubi_verifyExistFiles() {
  dataLog("Verificando que existan los archivos de los sensores en la SD", 0);
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
    dataLog("Sincronizando datos de Oxigenacion", 0);
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
    dataLog("Sincronizando datos de Temperatura", 0);
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
    dataLog("Sincronizando archivos de Conductividad", 0);
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
    dataLog("Sincronizando archivos de PH", 0);
    delay(100);
    lcd.clear();
  }

  GetAllValues();
}

void readFileLineByLine(fs::FS &fs, const char *path, const char *var) {
  //Serial.printf("Reading file: %s\n", path);
  int c = 0;
  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  //char* ubiContext = "\"estado\":\"resuvido\"";
  char *ubiContext = "";
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
  Serial.print("N??mero de elementos sincronizados: ");
  Serial.println(c);
  file.close();
  deleteFile(SD, path);
}

void Connect() {
  GetAllValues();
  const long interval = 2000;
  dataLog("Conectandose a  wifi" + String(WIFISSID.c_str()), 0);
  WiFi.begin(WIFISSID.c_str(), PASSWORD.c_str());
  lcd.setCursor(18, 3);
  lcd.print("  ");
  desconexionwifi();
  int contador = 0;
  unsigned long currentMillis = millis();
  while (WiFi.status() != WL_CONNECTED) {
    dataLog("No se pudo conectar a red WIFI", 0);
    showLocalValues();
    GetAllValuesIntervals();
    if (abs(millis() - currentMillis) > interval) {
      dataLog("Intentando conectarse a red WIFI", 0);
      Serial.print(".");
      contador++;
      currentMillis = millis();
    }
    if (contador > 5) {
      WiFi.disconnect();
      contador = 0;
      delay(100);
      dataLog("Intentando conectarse a Wifi", 0);
      WiFi.begin(WIFISSID.c_str(), PASSWORD.c_str());
      //ESP.restart();
    }
  }
  Serial.println(WiFi.localIP());
  lcd.setCursor(0, 3);
  dataLog("Conectado a WIFI", 0);
  lcd.print("IP: ");
  lcd.setCursor(4, 3);
  lcd.print(WiFi.localIP());
  serverSetup();
  while (WiFi.status() == WL_CONNECTED) {
    showLocalValues();
    GetAllValuesIntervals();

    if (abs(millis() - currentMillis) > interval) {
      conexionwifi();
      /*lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Conexion a red  ");*/
      /*if (ubidots.reconnect()) {
        break;
      }*/
      desconexionwifi();
      currentMillis = millis();
      /*lcd.setCursor(0, 1);
      lcd.print(" sin internet    ");*/
    }
  }
  delay(500);
  //lcd.clear();
  lcd.setCursor(19, 3);
  lcd.print(" ");
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
    if (abs(millis() - timer_pullup) > interval_pullup) {
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

  if (abs(millis() - timer_local_check) > interval_local_check) {
    lvOx = GetOxygen();
    lvTe = temperaturaLoop();
    lvCo = conductivityLoop();
    lvPh = phloop();
    //lcd.clear();
    //oxygenValue = GetOxygen();
    if (lvOx == 0) {

    } else if (lvOx <= minOxy) {
      dataLog("Oxigenacion en nivel minimo = " + String(lvOx), 0);
      lcd.createChar(1, customChar1);
      lcd.setCursor(2, 0);
      lcd.write(1);
      digitalWrite(R1, LOW);
      digitalWrite(R2, LOW);
      dataLog("Activacion de blower", 0);
      BlowerState = false;
      Serial.print("Blower ");
      Serial.println(BlowerState);
      //timer_pullup = millis();
      if (ubidots.connected()) {
        ubidots.add("control1", 1);
        ubidots.publish(DEVICE_LABEL);
      }
    } else if (lvOx >= maxOxy) {
      dataLog("Oxigenacion en nivel maximo = " + String(lvOx), 0);
      lcd.createChar(0, customChar);
      lcd.setCursor(2, 0);
      lcd.write(0);
      dataLog("Desactivacion de blower", 0);
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
    lcd.print(String(lvOx) + "  ");
    dataLog("Mostrando valor de Oxigenacion en LCD", 0);


    lcd.setCursor(0, 1);
    lcd.print("Co");
    lcd.setCursor(3, 1);
    lcd.print(lvCo);
    dataLog("Mostrando valor de conductivida en LCD", 0);

    if (lvCo <= minConduct) {
      dataLog("Conductividad al minimo = " + String(lvCo), 0);
      lcd.createChar(4, customChar1);
      lcd.setCursor(2, 1);
      lcd.write(4);
    }
    if (lvCo >= maxConduct) {
      dataLog("Conductividad al maximo = " + String(lvCo), 0);
      lcd.createChar(3, customChar);
      lcd.setCursor(2, 1);
      lcd.write(3);
    }
    if (lvCo >= minConduct && lvCo <= maxConduct) {
      dataLog("Conductividad Estable", 0);
      lcd.setCursor(2, 1);
      lcd.print(" ");
    }
    lcd.setCursor(12, 0);
    lcd.print("Te");
    lcd.setCursor(15, 0);
    dataLog("Mostrando Temperatura en LCD", 0);
    lcd.print(lvTe, 2);

    if (lvTe >= maxTemp && flag21 == false) {
      dataLog("Temeratura alta = " + String(lvTe), 0);
      activarbocina();
      lcd.setCursor(14, 0);
      lcd.print(" ");
      lcd.createChar(0, customChar);
      lcd.setCursor(14, 0);
      lcd.write(0);
    } else if (lvTe <= minTemp && flag21 == false) {
      dataLog("Temperatura baja = " + String(lvTe), 0);
      activarbocina();
      lcd.setCursor(14, 0);
      lcd.print(" ");
      lcd.createChar(1, customChar1);
      lcd.setCursor(14, 0);
      lcd.write(1);
    } else if (lvTe >= minTemp && lvTe <= maxTemp && flag21 == true) {
      dataLog("Temperatura OK", 0);
      desactivarbocina();
      lcd.setCursor(14, 0);
      lcd.print(" ");
    }

    if (lvPh <= minPH) {
      dataLog("PH al minimo = " + String(lvPh), 0);
      lcd.createChar(4, customChar1);
      lcd.setCursor(14, 1);
      lcd.write(4);
    } else if (lvPh >= maxPH) {
      dataLog("PH al maximo = " + String(lvPh), 0);
      lcd.createChar(4, customChar);
      lcd.setCursor(14, 1);
      lcd.write(4);
    } else {
      lcd.setCursor(14, 1);
      dataLog("PH OK", 0);
      lcd.print(" ");
    }

    lcd.setCursor(12, 1);
    lcd.print("PH");
    lcd.setCursor(15, 1);
    lcd.print(lvPh);

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
  dataLog("Bocina Activada", 0);
  digitalWrite(bocina, HIGH);
  flag21 = true;
}

void desactivarbocina() {
  dataLog("Bocina desactivada", 0);
  digitalWrite(bocina, LOW);
  flag21 = false;
}
void getAllPreferences() {
  dataLog("Extraccion de preferencias", 0);
  preferences.begin("network", false);
  WIFISSID = preferences.getString("ssid");
  PASSWORD = preferences.getString("password");
  preferences.end();

  preferences.begin("data", false);
  deviceName = preferences.getString("name");
  deviceType = preferences.getString("type");
  interval_save_data = preferences.getUInt("interval");
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

  preferences.begin("valuesOx", false);
  lvOx = preferences.getFloat("o5");
  preferences.end();
  preferences.begin("valuesTe", false);
  lvTe = preferences.getFloat("t5");
  preferences.end();
  preferences.begin("valuesPh", false);
  lvPh = preferences.getFloat("ph5");
  preferences.end();
  preferences.begin("valuesCon", false);
  lvCo = preferences.getFloat("c5");
  preferences.end();
  /*
  Serial.println("SSID: " + WIFISSID);
  Serial.println("PASS: " + PASSWORD);
  Serial.println("Conductivity min: " + String(minConduct));
  Serial.println("Conductivity max: " + String(maxConduct));
  Serial.println("Oxygen min: " + String(minOxy));
  Serial.println("Oxygen max: " + String(maxOxy));
  Serial.println("PH min: " + String(minPH));
  Serial.println("PH max: " + String(maxPH));
  Serial.println("Temperature min: " + String(minTemp));
  Serial.println("Temperature max: " + String(maxTemp));*/
}
void conexionwifi() {
  if (WiFi.RSSI() == 0) {
    lcd.setCursor(18, 3);
    lcd.print(" ");
  } else if (WiFi.RSSI() >= -25) {
    lcd.createChar(5, wifi1);
    lcd.setCursor(18, 3);
    lcd.write(5);
  } else if (WiFi.RSSI() >= -50) {
    lcd.createChar(5, wifi2);
    lcd.setCursor(18, 3);
    lcd.write(5);
  } else if (WiFi.RSSI() >= -75) {
    lcd.createChar(5, wifi3);
    lcd.setCursor(18, 3);
    lcd.write(5);
  } else if (WiFi.RSSI() >= -100) {
    lcd.createChar(5, wifi4);
    lcd.setCursor(18, 3);
    lcd.write(5);
  }
}
void desconexionwifi() {
  lcd.createChar(6, wifi5);
  lcd.setCursor(19, 3);
  lcd.write(6);
  /*delay(500);
  lcd.setCursor(19, 3);
  lcd.print(" ");
  delay(500);*/
}
