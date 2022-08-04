#define VARIABLE_LABEL_SUB_1 "control1"    // Assing the variable label to subscribe
/*
#define VARIABLE_LABEL_SUB_2 "control2"   // Assing the variable label to subscribe
#define VARIABLE_LABEL_SUB_3 "control3"   // Assing the variable label to subscribe*/

#define R1 13
#define R2 14
//#define R3 4
int valorEncendido = 0;
int valorApagado = 1;

const uint8_t NUMBER_OF_VARIABLES = 1;// Number of variable to subscribe to
char *variable_labels[NUMBER_OF_VARIABLES] = {"control1"/*, "control2", "control3"*/}; // labels of the variable to subscribe to

float CONTROL1;  // Name of the variable to be used within the code.
/*float CONTROL2;  // Name of the variable to be used within the code.
float CONTROL3;  // Name of the variable to be used within the code.*/

float value;      // To store incoming value.
uint8_t variable; // To keep track of the state machine and be able to use the switch case.
#define bocina  0
#define btnServer 15      //Pin for increment left
boolean btnServerState = false;

#define btnBlower 4      //Pin for increment left
boolean btnBlowerState = false;
boolean BlowerState = false;

const int ERROR_VALUE = 65535; // Set here an error value

void callback(char *topic, byte *payload, unsigned int length)
{
  char *variable_label = (char *)malloc(sizeof(char) * 30);
  get_variable_label_topic(topic, variable_label);
  Serial.println(variable_label);
  value = btof(payload, length);
  set_state(variable_label);
  if (value == 1) {
    value = valorEncendido;
  } else if (value == 0) {
    value = valorApagado;
  }
  execute_cases();
  free(variable_label);
}

// Parse topic to extract the variable label which changed value
void get_variable_label_topic(char *topic, char *variable_label)
{
  Serial.print("topic:");
  Serial.println(topic);
  sprintf(variable_label, "");
  for (int i = 0; i < NUMBER_OF_VARIABLES; i++)
  {
    char *result_lv = strstr(topic, variable_labels[i]);
    if (result_lv != NULL)
    {
      uint8_t len = strlen(result_lv);
      char result[100];
      uint8_t i = 0;
      for (i = 0; i < len - 3; i++)
      {
        result[i] = result_lv[i];
      }
      result[i] = '\0';
      Serial.print("Label is: ");
      Serial.println(result);
      sprintf(variable_label, "%s", result);
      break;
    }
  }
}

// cast from an array of chars to float value.
float btof(byte *payload, unsigned int length)
{
  char *demo_ = (char *)malloc(sizeof(char) * 10);
  for (int i = 0; i < length; i++)
  {
    demo_[i] = payload[i];
  }
  return atof(demo_);
}

// State machine to use switch case
void set_state(char *variable_label)
{
  variable = 0;
  for (uint8_t i = 0; i < NUMBER_OF_VARIABLES; i++)
  {
    if (strcmp(variable_label, variable_labels[i]) == 0)
    {
      break;
    }
    variable++;
  }
  if (variable >= NUMBER_OF_VARIABLES)
    variable = ERROR_VALUE; // Not valid
}

// Function with switch case to determine which variable changed and assigned the value accordingly to the code variable
void execute_cases()
{
  switch (variable)
  {
    case 0:
      CONTROL1 = value;
      digitalWrite(R1, value);
      digitalWrite(R2, value);
      if (value == valorEncendido) {
        BlowerState = true;
      }else if(value == valorApagado){
        BlowerState = false;
      }
      Serial.print("CONTROL1: ");
      Serial.println(CONTROL1);
      Serial.println();
      break;
    /*case 1:
      CONTROL2 = value;
      digitalWrite(R2, value);
      digitalWrite(R1, value);
      Serial.print("CONTROL2: ");
      Serial.println(CONTROL2);
      Serial.println();
      break;
    case 2:
      CONTROL3 = value;
      //digitalWrite(R3, value);
      Serial.print("CONTROL3: ");
      Serial.println(CONTROL3);
      Serial.println();
      break;*/
    case ERROR_VALUE:
      Serial.println("error");
      Serial.println();
      break;
    default:
      Serial.println("default");
      Serial.println();
  }
}

void ubi_controlSetup() {
  pinMode(bocina, OUTPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  //pinMode(R3, OUTPUT);
  digitalWrite(bocina, LOW);
  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
  //digitalWrite(R3, LOW);
}
