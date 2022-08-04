#include <OneWire.h>
#include <DallasTemperature.h>

OneWire ourWireBus(32);                  //Se establece el pin 4  como bus OneWire
DallasTemperature sensor(&ourWireBus); //Se declara una variable u objeto para nuestro sensor
float temp1 = 0;
void setuptemperatura()
{
  sensor.begin();   //Se inicia el sensor
}
float temperaturaLoop() {
  sensor.requestTemperatures();   //Se envía el comando para leer la temperatura
  temp1 = sensor.getTempCByIndex(0); //Se obtiene la temperatura en ºC
  return temp1;
  //temp = random(0,50); //Se obtiene la temperatura en ºC
}
