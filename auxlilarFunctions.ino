
byte customChar[] = { //flecha hacia arriba
  B00000,
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B00100,
  B00000
};
byte customChar1[] = {//flecha hacia abajo
  B00000,
  B00100,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100,
  B00000
};
byte wifi[] = {//Wifi
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B10000,
  B10000,
  B00000
};
byte wifi1[] = {//Wifi
  B00000,
  B00000,
  B00000,
  B00000,
  B01000,
  B11000,
  B11000,
  B00000
};
byte wifi2[] = {//Wifi
  B00000,
  B00000,
  B00000,
  B00100,
  B01100,
  B11100,
  B11100,
  B00000
};
byte wifi3[] = {//Wifi
  B00000,
  B00000,
  B00010,
  B00110,
  B01110,
  B11110,
  B11110,
  B00000
};
byte wifi4[] = {//Wifi
  B00000,
  B00001,
  B00011,
  B00111,
  B01111,
  B11111,
  B11111,
  B00000
};
byte wifi5[] = {//NO_Wifi
  B00000,
  B10001,
  B01010,
  B00100,
  B01010,
  B10001,
  B00000,
  B00000
};

void OxygenNewValue(float newOxygenValue){
    preferences.begin("valuesOx", false);
    preferences.putFloat("o5", preferences.getFloat("o4"));
    preferences.putFloat("o4", preferences.getFloat("o3"));
    preferences.putFloat("o3", preferences.getFloat("o2"));
    preferences.putFloat("o2", preferences.getFloat("o1"));
    preferences.putFloat("o1", newOxygenValue);
    preferences.end();
}
void TempNewValue(float newTempValue){
    preferences.begin("valuesTe", false);
    preferences.putFloat("t5", preferences.getFloat("t4"));
    preferences.putFloat("t4", preferences.getFloat("t3"));
    preferences.putFloat("t3", preferences.getFloat("t2"));
    preferences.putFloat("t2", preferences.getFloat("t1"));
    preferences.putFloat("t1", newTempValue);
    preferences.end();
}
void ConNewValue(float newConValue){
    preferences.begin("valuesCon", false);
    preferences.putFloat("c5", preferences.getFloat("c4"));
    preferences.putFloat("c4", preferences.getFloat("c3"));
    preferences.putFloat("c3", preferences.getFloat("c2"));
    preferences.putFloat("c2", preferences.getFloat("c1"));
    preferences.putFloat("c1", newConValue);
    preferences.end();
}
void PhNewValue(float newPhValue){
    preferences.begin("valuesPh", false);
    preferences.putFloat("ph5", preferences.getFloat("ph4"));
    preferences.putFloat("ph4", preferences.getFloat("ph3"));
    preferences.putFloat("ph3", preferences.getFloat("ph2"));
    preferences.putFloat("ph2", preferences.getFloat("ph1"));
    preferences.putFloat("ph1", newPhValue);
    preferences.end();
}
void newDateValue(String date) {
  preferences.begin("valuesD", false);
  preferences.putString("d1_5", preferences.getString("d1_4"));
  preferences.putString("d1_4", preferences.getString("d1_3"));
  preferences.putString("d1_3", preferences.getString("d1_2"));
  preferences.putString("d1_2", preferences.getString("d1_1"));
  preferences.putString("d1_1", date);
  preferences.end();
}
