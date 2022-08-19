////////--------------Timers-------------//////////////
unsigned long timer_conductivity; //Temporizador que lleva el tiempo de vida del programa
unsigned long timer_temperature; //Temporizador que lleva el tiempo de vida del programa
unsigned long timer_oxygen; //Temporizador que lleva el tiempo de vida del programa
unsigned long timer_ph; //Temporizador que lleva el tiempo de vida del programa
unsigned long timer_local_check = 25000;; //Temporizador que lleva el tiempo de vida del programa
unsigned long timer_sensors_loop; //Temporizador que lleva el tiempo de vida del programa
unsigned long timer_to_start_server; //Temporizador que lleva el tiempo de vida del programa
unsigned long timer_pullup; //Temporizador que lleva el tiempo de vida del programa
unsigned long timer_ten_minutes; //Temporizador que lleva el tiempo de vida del programa

////////--------------Intervalos-------------//////////////
const int interval_conductivity = 5000;
const int interval_temperature = 10000;
const int interval_oxygen = 300000;
const int interval_ph = 300000;
const int interval_local_check = 1000;
const int interval_sensors_loop = 20000;
const int interval_to_start_server = 10000;
const int interval_pullup = 1000;
const int interval_ten_minutes = 600000;//Tiempo para almacenar los valores en la memoria flash
const int PUBLISH_FREQUENCY = 900000; //Tiempo para subir los valores (15 min)
const int CHECK_OXYGEN_FRECUENCY = 300000; //Tiempo para checar los valores (5 min)

////////--------------Variables-------------//////////////
const char* Oxygen = "oxygen";
const char* Conductivity = "conductivity";
const char* Temperature = "temperature";
const char* Ph = "ph";
bool connectedW = false;
bool reconnectedW = false;
