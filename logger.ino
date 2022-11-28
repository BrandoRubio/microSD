void Verification(){

  DateTime now = rtc.now();
  char buf2[] = "MM-DD-YYYY";
  String Name = now.toString(buf2);
  String nameFolder = "/Logger/"+ Name +".csv"; 

  if(FileExist(SD,nameFolder.c_str())== 1) //SI SE REINICIA EL ESP32, INDICA EL PUNTO DE RESET.
  {
    appendFile(SD, nameFolder.c_str() , "Inicializando....... \n ");
  }
}

//ESCRITURA EN MEMORIA SD
void dataLog(String instruction, int level)  // Registro de datos en archivo
{ 
  String Niv []= {"[INFO] ","[ERROR] "}; 
  // INFO - informacion de la tarea, Error -- Error de ejecucion

  DateTime now = rtc.now();
  //BUFFER HORAS-MINUTOS-SEGUNDOS
  char buf1[] = "hh:mm:ss";
  String Time = now.toString(buf1);

  char buf3[] = "MM-DD-YYYY";
  String Name = now.toString(buf3);
  String nameFolder = "/Logger/"+ Name +".csv"; 
  String text = Time + ",:-->,"+ Niv[level] + "," + instruction + "\n";
  
  if(FileExist(SD,nameFolder.c_str())== 0) // SI EL ARCHIVO NO EXISTE, CREA UNO NUEVO
  {
     writeFile(SD, nameFolder.c_str(),"Hora, ,Nivel,Instruccion\r\n "); 
     appendFile(SD, nameFolder.c_str() , text.c_str()); 
  }  
  else //SI YA EXISTE, AGREGA LOS DATOS EN EL ARCHIVO
  {
    appendFile(SD, nameFolder.c_str() , text.c_str()); 
  }
}
