#include <Preferences.h>
Preferences preferences;
void setup()
{
  Serial.begin(115200);
  lcdSetup();
  setupSD();
  setupClock();
  Verification();
  setupConductivity();
  setuptemperatura();
  ubi_controlSetup();
  ubi_mainSetup();
}

| loop()
{
  ubi_mainLoop();
}
