void setup()
{
  Serial.begin(115200);
  lcdSetup();
  setupSD();
  setupClock();
  setupConductivity();
  setuptemperatura();
  ubi_controlSetup();
  ubi_mainSetup();
}

void loop()
{
  ubi_mainLoop();
}
