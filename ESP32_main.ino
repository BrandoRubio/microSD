void setup()
{
  Serial.begin(115200);
  lcdSetup();
  setupSD();
  setupClock();
  setupConductivity();
  ubi_controlSetup();
  ubi_mainSetup();
  //serverSetup();
}

void loop()
{
  ubi_mainLoop();
}
