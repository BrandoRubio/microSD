//po = 39
/*
 * PH       ESP32
 *   To     NC        
 *   Do     NC
 *   Po     VP        GRIS
 *   G      GND       BLANCO
 *   G      GND       NARANJA
 *   V+     VIN       MORADO
 * 
 */
float PH;
int cont;

float calibration = 15.75; //valor de b para calibración sensor pH //9
const int analogInPin = 36; //pin sensor pH
int sensorValue = 0; 
unsigned long int avgValue; 
float b;
int buf[10],temp11;

float obtenerpH(){
  for(int i=0;i<10;i++) 
  { 
    buf[i]=analogRead(analogInPin);
    delay(30);
  }
  for(int i=0;i<9;i++)
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp11=buf[i];
        buf[i]=buf[j];
        buf[j]=temp11;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)
    avgValue+=buf[i];
  float pHVol=((((float)avgValue)*5.0)/4095.0)/6;
  float phValue = (-2.5 * pHVol) + calibration;
  return(phValue);
}


float phloop()
{

  PH=obtenerpH();
  return PH;
}
